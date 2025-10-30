/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 10:46:35 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/30 11:15:04 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

# include "Server.hpp"
# include "Request.hpp"
# include "MethodExecutor.hpp"
# include "CodeDefines.h"
# include <sys/epoll.h>
# include "Context.hpp"
# include <fcntl.h>
# include <errno.h>
# include <string.h>
# include <unistd.h>
# include <cstring>

# include <sys/signal.h>
# include <netdb.h>
# include <sys/epoll.h>




#define MAX_EVENT_WAITED 1024
#define NB_EVENTS 1024
#define BUFFER_SIZE 1024

int setNonBlocking(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "fcntl: " << strerror(errno) <<std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int my_epoll_ctl(int epollfd, int op, uint32_t events, int fd) {
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, op, fd, &ev) == -1) {
		std::cerr << "epoll_ctl: " << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

/**
 * @note If the modification to EPOLLOUT fails, the connection will be closed
 */
int queueResponse(Client &client, std::string& response, int epollfd) {
	if (my_epoll_ctl(epollfd, EPOLL_CTL_MOD, EPOLLIN | EPOLLOUT, client.getFd()) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	client.setSendBuffer(response);
	return (EXIT_SUCCESS);
}

#include <vector>

/**
 * - Sets up a TCP server socket
 * - Binds it to the specified address and port
 * - Listens for incoming connections
 * STRESS TESTED
 */
int initServerFds(Server& server) {
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
			std::cout << "Trying to bind to " << ((struct sockaddr_in *)curraddr->ai_addr)->sin_addr.s_addr << ":" << ntohs(((struct sockaddr_in *)curraddr->ai_addr)->sin_port) << std::endl;
			
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
			curraddr = curraddr->ai_next;
		}
		freeaddrinfo(addrinfos);
	}
	return (EXIT_SUCCESS);
}

/**
 * @note If any step of adding the client to epoll fails, the connection is closed and the client is not added to the server
 */
int addClient(Server &server, int servfd, int epollfd) {
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
	std::cout << "New connection, fd: " << clientfd << std::endl;
	return (EXIT_SUCCESS);
}

/**
 * @note If any error occurs, the function returns EXIT_FAILURE to signal the caller to delete the client
 * Possible errors:
 * 	- readPacket fail -> client disconnected or read error -> delete the client
 * 	- parsePacket fail -> send Bad Request
 * 	- MethodExecutor fail -> Fabien handles it
 * 	- queueResponse fail -> send error -> delete the client
 */
int handleClientIn(Server& server, Client& client, int epollfd) {

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

int initEpoll(std::vector<Server>& servers) {
	int epollfd = epoll_create(1);
	if (epollfd == -1) {
		std::cerr << "epoll_create: " << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}

	// add each sockfd of each server to epoll
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
		std::vector<int> servfds = it->getSockfds();
		for (std::vector<int>::iterator fd_it = servfds.begin(); fd_it != servfds.end(); ++fd_it) {
			if (my_epoll_ctl(epollfd, EPOLL_CTL_ADD, EPOLLIN, *fd_it) == -1) {
				close(epollfd);
				return (EXIT_FAILURE);
			}
		}
	}
	return (epollfd);
}

void sigint_handler(int sig) {
	(void)sig;
	write(2, "\n", 1);
}

int launchEpoll(Context &ctx) {
	std::vector<Server>::iterator it;
	for (it = ctx.getServers().begin(); it != ctx.getServers().end(); ++it) {
		if (initServerFds(*it) == EXIT_FAILURE)
			return (EXIT_FAILURE);
	}
	
	int epollfd = initEpoll(ctx.getServers());
	if (epollfd == EXIT_FAILURE) {
		return (EXIT_FAILURE);
	}
	ctx.setEpollFd(epollfd);

	struct epoll_event events[NB_EVENTS];
	int eventsReady;

	if (signal(SIGINT, &sigint_handler) == SIG_ERR) {
		std::cerr << "signal: " << strerror(errno) << std::endl;
		close(epollfd);
		return (EXIT_FAILURE);
	}

	while (1) {
		printf("epoll_waiting\n");
		eventsReady = epoll_wait(epollfd, events, MAX_EVENT_WAITED, -1);
		if (eventsReady == -1) {
			std::cerr << "epoll_wait: " << strerror(errno) << std::endl;
			close(epollfd);
			return (EXIT_FAILURE);
		}
		std::cout << eventsReady << " event(s)" << std::endl;
		for (int i = 0; i < eventsReady; ++i) {
			Server& server = ctx.getRelatedServer(events[i].data.fd);
			if (ctx.isListenerFd(events[i].data.fd)) {
				if (addClient(server, events[i].data.fd, epollfd) == EXIT_FAILURE) {
					continue;
				}
			} else {
				Client &client = server.getClient(events[i].data.fd);
				if (events[i].events & EPOLLIN) {
					std::cout << "EPOLLIN fd: " << client.getFd() << std::endl;
					if (handleClientIn(server, client, epollfd) == EXIT_FAILURE) {
						server.deleteClient(client.getFd());
						continue;
					}
				}
				if (events[i].events & EPOLLOUT) {
					std::cout << "EPOLLOUT fd: " << client.getFd() << std::endl;
					if (client.sendPendingResponse(epollfd) == EXIT_FAILURE) {
						server.deleteClient(client.getFd());
						continue;
					}
				}
			}
		}
		std::cout << "-----\n";
	}
	return 0;
}
