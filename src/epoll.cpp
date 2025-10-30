/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 10:46:35 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/30 14:49:15 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

# include "Server.hpp"
# include "Request.hpp"
# include "MethodExecutor.hpp"
# include "CodeDefines.h"
# include "defines.h"
# include "Context.hpp"

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

static int initServerFds(Server& server);
static int initEpoll(Context &ctx);
static int addClient(Server &server, int servfd, int epollfd);
static int queueResponse(Client &client, std::string& response, int epollfd);
static int handleClientIn(Server& server, Client& client, int epollfd);

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
		if (PRINT)
			std::cout << "epoll_waiting" << std::endl;
		eventsReady = epoll_wait(ctx.getEpollFd(), events, NB_EVENTS, -1); //TODO timeout return 0
		if (eventsReady == -1) {
			std::cerr << "epoll_wait: " << strerror(errno) << std::endl;
			return (EXIT_FAILURE);
		}
		if (PRINT)
			std::cout << eventsReady << " event(s)" << std::endl;
		for (int i = 0; i < eventsReady; ++i) {
			Server& server = ctx.getRelatedServer(events[i].data.fd);
			if (ctx.isListenerFd(events[i].data.fd)) {
				if (addClient(server, events[i].data.fd, ctx.getEpollFd()) == EXIT_FAILURE) {
					continue;
				}
			} else {
				Client& client = server.getClient(events[i].data.fd);
				if (events[i].events & EPOLLIN) {
					if (PRINT)
						std::cout << "EPOLLIN fd: " << client.getFd() << std::endl;
					if (handleClientIn(server, client, ctx.getEpollFd()) == EXIT_FAILURE) {
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
		if (PRINT)
			std::cout << "-----\n";
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
			if (PRINT)
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
			if (PRINT)
				std::cout << "OK" << std::endl;
			curraddr = curraddr->ai_next;
		}
		freeaddrinfo(addrinfos);
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

/**
 * @note If any error occurs, the function returns EXIT_FAILURE to signal the caller to delete the client
 */
static int handleClientIn(Server& server, Client& client, int epollfd) {

	if (client.readPacket() == EXIT_FAILURE)
		return (EXIT_FAILURE);

	std::string response;
	
	try {
		client.parsePacket(server);
		if (client.getStatus() == WAITING)
			return (EXIT_SUCCESS);
		response = MethodExecutor(server, client).execute();
	} catch (const RequestException& re) {
		std::cerr << re.what() << std::endl;
		response = Response("HTTP/1.1", server.getErrorPageByCode(re.getCode())).build();
	} catch (const std::exception& e) {
		std::cerr << "Unhandled exception (should never happen): " << e.what() << std::endl;
		response = Response("HTTP/1.1", server.getErrorPageByCode(INTERNAL_SERVER_ERROR)).build();
	}

	return (queueResponse(client, response, epollfd) == EXIT_FAILURE);
}

/**
 * @note If the modification to EPOLLOUT fails, the connection will be closed
 */
static int queueResponse(Client &client, std::string& response, int epollfd) {
	if (my_epoll_ctl(epollfd, EPOLL_CTL_MOD, EPOLLIN | EPOLLOUT, client.getFd()) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	client.setSendBuffer(response);
	return(EXIT_SUCCESS);
}
