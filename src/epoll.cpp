/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 10:46:35 by mniemaz           #+#    #+#             */
/*   Updated: 2025/11/06 14:24:04 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

# include "Server.hpp"
# include "Request.hpp"
# include "MethodExecutor.hpp"
# include "CodeDefines.h"
# include "defines.h"
# include "Context.hpp"
# include "Error.hpp"

# include <sys/wait.h>
# include <sys/epoll.h>
# include <fcntl.h>
# include <errno.h>
# include <string.h>
# include <unistd.h>
# include <cstring>
# include <netdb.h>
# include <sys/epoll.h>
# include <sys/signal.h>


#define NB_EVENTS 1024
#define BUFFER_SIZE 1024

bool isCGI(Request &req, Location &loc);
int CGIHandler(Request &req, Location &loc, Server &serv, Client &client, Context& ctx);


static int initServerFds(Server& server);
static int initEpoll(Context &ctx);
static int addClient(Server &server, int servfd, int epollfd);
static int handleClientIn(Server& server, Client& client, Context& ctx);
int queueResponse(Client &client, std::string& response, int epollfd);

int launchEpoll(Context &ctx) {
	std::vector<Server>::iterator it;
	for (it = ctx.getServers().begin(); it != ctx.getServers().end(); ++it) {
		if (initServerFds(*it) == EXIT_FAILURE)
			return (EXIT_FAILURE);
	}

	if (initEpoll(ctx) == EXIT_FAILURE) {
		return (EXIT_FAILURE);
	}

	struct epoll_event events[NB_EVENTS];
	int eventsReady;

	if (signal(SIGINT, &sigint_handler) == SIG_ERR) {
		std::cerr << "signal: " << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}
	
	while (1) {
		eventsReady = epoll_wait(ctx.getEpollFd(), events, NB_EVENTS, 1000);
		if (eventsReady == -1) {
			std::cerr << "epoll_wait: " << strerror(errno) << std::endl;
			return (EXIT_FAILURE);
		}
		for (int i = 0; i < eventsReady; ++i) {
			if (PRINT)
				std::cout << "-----\n";
			if (ctx.isRunningCGI(events[i].data.fd)) {
				ctx.handleEventCgi(events[i].data.fd);
			}
			else
			{
				Server* servptr = ctx.getRelatedServer(events[i].data.fd);
				if (servptr == NULL) {
					std::cerr << "No related server for fd " << events[i].data.fd << std::endl;
					continue;
				}
				Server& server = *servptr;
				if (ctx.isListenerFd(events[i].data.fd)) {
					if (addClient(server, events[i].data.fd, ctx.getEpollFd()) == EXIT_FAILURE) {
						continue;
					}
				} else {
					Client& client = server.getClient(events[i].data.fd);
					if (events[i].events & EPOLLIN) {
						if (PRINT)
							std::cout << "EPOLLIN fd: " << client.getFd() << std::endl;
						if (handleClientIn(server, client, ctx) == EXIT_FAILURE) {
							server.deleteClient(client.getFd());
							continue;
						}
					}
					if (events[i].events & EPOLLOUT) {
						if (PRINT)
							std::cout << "EPOLLOUT fd: " << client.getFd() << std::endl;
						if (client.sendPendingResponse(ctx.getEpollFd()) == EXIT_FAILURE) {
							server.deleteClient(client.getFd());
							continue;
						}
					}
				}
			}
		}
		ctx.checkTimedOutCGI();
		ctx.handleClientTimeoutsAndLinger();
	}
	return 0;
}

/**
 * @brief Initializes epoll and adds all server socket fds to it
 */
static int initEpoll(Context &ctx) {
	int epollfd = epoll_create(1);
	if (epollfd == -1) {
		std::cerr << "epoll_create: " << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}

	std::vector<Server>& servers = ctx.getServers();
	std::vector<Server>::iterator it;
	for (it = servers.begin(); it != servers.end(); ++it) {
		std::vector<int> servfds = it->getSockfds();
		for (std::vector<int>::iterator fd_it = servfds.begin(); fd_it != servfds.end(); ++fd_it) {
			if (my_epoll_ctl(epollfd, EPOLL_CTL_ADD, EPOLLIN, *fd_it) == -1) {
				return (EXIT_FAILURE);
			}
		}
	}
	ctx.setEpollFd(epollfd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Initializes all server socket fds: creates, binds and listens
 */
static int initServerFds(Server& server) {
	struct addrinfo	*addrinfos;
	struct addrinfo	hints;
	int nbServersStarted;

	nbServersStarted = 0;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // ipv4
	hints.ai_socktype = SOCK_STREAM; // tcp

	std::vector<std::string>::const_iterator port;
	for (port = server.getPorts().begin(); port < server.getPorts().end(); ++port) {
		int gai_ret = getaddrinfo(	server.getHost().c_str(),
									port->c_str(),
									&hints,
									&addrinfos);
		if (gai_ret != 0) {
			std::cerr << "getaddrinfo: " << gai_strerror(gai_ret) << std::endl;
			return (EXIT_FAILURE);
		}

		struct addrinfo *curraddr = addrinfos;
		while (curraddr) {
			std::cout << printIP(((struct sockaddr_in *)curraddr->ai_addr)->sin_addr.s_addr) << ":" << ntohs(((struct sockaddr_in *)curraddr->ai_addr)->sin_port) << " -> ";

			int fd = socket(curraddr->ai_family,
							curraddr->ai_socktype,
							curraddr->ai_protocol);
			if (fd == -1) {
				std::cerr << "socket: " << strerror(errno) << std::endl;
				close(fd);
				curraddr = curraddr->ai_next;
				continue;
			}
			
			if (setNonBlocking(fd) == EXIT_FAILURE) {
				close(fd);
				curraddr = curraddr->ai_next;
				continue;
			}
			
			int one = 1;
			int sso_ret = setsockopt(fd,
									SOL_SOCKET,
									SO_REUSEADDR,
									&one,
									sizeof(one));
			if (sso_ret == -1) {
				std::cerr << "setsockopt: " << strerror(errno) << std::endl;
				close(fd);
				curraddr = curraddr->ai_next;
				continue;
			}

			if (bind(fd, curraddr->ai_addr, curraddr->ai_addrlen) == -1) {
				std::cerr << "bind: " << strerror(errno) << std::endl;
				close(fd);
				curraddr = curraddr->ai_next;
				continue;
			}
			
			if (listen(fd, 1024) == -1) {
				std::cerr << "listen: " << strerror(errno) << std::endl;
				close(fd);
				curraddr = curraddr->ai_next;
				continue;
			}
			server.addSockfd(fd);
			++nbServersStarted;
			std::cout << "OK" << std::endl;
			curraddr = curraddr->ai_next;
		}
		freeaddrinfo(addrinfos);
	}
	if (nbServersStarted == 0) {
		std::cerr << "No servers could be started, exiting." << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

/**
 * @note If any step of adding the client to epoll fails, the connection is closed and the client is not added to the server
 */
static int addClient(Server &server, int servfd, int epollfd) {
	struct sockaddr clientAddr;
	int addrLen = sizeof(clientAddr);
	int clientfd = accept(servfd, (struct sockaddr *)&clientAddr, (socklen_t *)&(addrLen));
	if (clientfd == -1) {
		std::cerr << "accept: " << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}

	if (setNonBlocking(clientfd) == EXIT_FAILURE) {
		close(clientfd);
		return (EXIT_FAILURE);
	}
	if (my_epoll_ctl(epollfd, EPOLL_CTL_ADD, EPOLLIN, clientfd) == -1) {
		close(clientfd);
		return (EXIT_FAILURE);
	}

	Client cl(clientfd);
	server.addClient(cl);
	if (PRINT)
		std::cout << "New client: fd " << clientfd << std::endl;
	return (EXIT_SUCCESS);
}

static Location getRequestLocation(Request &req, Server &serv) {
	std::string path(req.getUri());
	std::map<std::string, Location> &locations = serv.getLocations();

	std::map<std::string, Location>::iterator it = locations.find(path);
	if (it != locations.end())
		return it->second;
	size_t l = path.find_last_of('/');
	if (l == 0)
		path = "/";
	else if (l != std::string::npos)
		path = path.substr(0, l);
	while (1) {
		for (std::map<std::string, Location>::iterator it = locations.begin();
			it != locations.end(); it++) {
			if (it->first == path)
				return it->second;
		}
		if (path.empty() || path == "/")
			break;

		l = path.find_last_of('/');
		if (l == 0)
			path = "/";
		else if (l != std::string::npos)
			path = path.substr(0, l);
		else
			break;
	}
	Location loc;
	loc.setCode(PAGE_NOT_FOUND);
	return loc;
}

/**
 * @return EXIT_FAILURE on error
 * @note LINGERING mode, if the client is in lingering status, this function just read 
 * packets the client is sending in order to gain time (see Client.hpp)
 * @note If any error occurs, the client connection has to be closed
 */
static int handleClientIn(Server& server, Client& client, Context& ctx) {

	int rdret = client.readPacket();
	if (rdret == EXIT_FAILURE)
		return (EXIT_FAILURE);
	else if (rdret == LINGERING)
		return (EXIT_SUCCESS);

	std::string response;
	
	try {
		client.parsePacket(server);
		if (client.getStatus() == WAITING)
			return (EXIT_SUCCESS);

		Request& req = client.getRequest();
		Location loc = getRequestLocation(req, server);
		if (loc.getCode() == PAGE_NOT_FOUND)
			response = Response(req.getVersion(), server.getErrorPageByCode(PAGE_NOT_FOUND)).build();
		else if (isCGI(req, loc)) {
			int cgiRet = CGIHandler(req, loc, server, client, ctx);
			if (cgiRet == DELETE_CLIENT)
				return (EXIT_FAILURE);
			if (cgiRet != CGI_PENDING)
				response = Response(req.getVersion(),
							server.getErrorPageByCode(cgiRet)).build();
			else
				return EXIT_SUCCESS;
		}
		else
			response = MethodExecutor(server, client).execute();
	} catch (const ParsePacketException& re) {
		std::cerr << re.what() << std::endl;
		client.setStatus(READY);
		response = Response("HTTP/1.1", server.getErrorPageByCode(re.getCode())).build();
	}

	return (queueResponse(client, response, ctx.getEpollFd()) == EXIT_FAILURE);
}

/**
 * @brief Queues the response to be sent to the client and modifies epoll to listen for EPOLLOUT events on the client fd
 * @note on EXIT_FAILURE returned, the client connection has to be closed
 */
int queueResponse(Client &client, std::string& response, int epollfd) {
	if (my_epoll_ctl(epollfd, EPOLL_CTL_MOD, EPOLLOUT, client.getFd()) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	client.setSendBuffer(response);
	return(EXIT_SUCCESS);
}
