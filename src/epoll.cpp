/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 10:46:35 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/28 14:25:28 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

#define MAX_EVENT_WAITED 1024
#define NB_EVENTS 1024
#define BUFFER_SIZE 1024

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
	return(EXIT_SUCCESS);
}

/**
 * - Sets up a TCP server socket
 * - Binds it to the specified address and port
 * - Listens for incoming connections
 * STRESS TESTED
 */
int createSocket(const Server &server) {
	int	servfd;
	struct addrinfo	*addrinfos;
	struct addrinfo	hints;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // ipv4 pelo
	hints.ai_socktype = SOCK_STREAM; // tcp
	hints.ai_flags = AI_PASSIVE; // for bind()

	int gai_ret = getaddrinfo(	server.getHost().c_str(),
						  		server.getPort().c_str(),
						  		&hints,
						  		&addrinfos);
	if (gai_ret != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(gai_ret) << std::endl;
		return (-1);
	}

	servfd = socket(addrinfos->ai_family,
					addrinfos->ai_socktype,
					addrinfos->ai_protocol);
	if (servfd == -1) {
		freeaddrinfo(addrinfos);
		std::cerr << "socket: " << strerror(errno) << std::endl;
		return (-1);
	}
	
	int opt = 1;
	int sso_ret = setsockopt(servfd,
							SOL_SOCKET,
							SO_REUSEADDR | SO_REUSEPORT,
							&opt,
							sizeof(opt));
	if (sso_ret == -1) {
		close(servfd);
		freeaddrinfo(addrinfos);
		std::cerr << "setsockopt: " << strerror(errno) << std::endl;
		return (-1);
	}		

	if (bind(servfd, addrinfos->ai_addr, addrinfos->ai_addrlen) == -1) {
		close(servfd);
		freeaddrinfo(addrinfos);
		std::cerr << "bind: " << strerror(errno) << std::endl;
		return (-1);
	}
	
	freeaddrinfo(addrinfos);
	if (listen(servfd, 2) == -1) {
		close(servfd);
		std::cerr << "listen: " << strerror(errno) << std::endl;
		return (-1);
	}
	return (servfd);
}

int setNonBlocking(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "fcntl: " << strerror(errno) <<std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/**
 * @note If any step of adding the client to epoll fails, the connection is closed and the client is not added to the server
 */
int addClient(Server& server, int servfd, int epollfd) {
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
 *  - queueResponse fail -> send error -> delete the client
 */
int handleClientIn(Server& server, Client& client, int epollfd) {

	if (client.readPacket() == EXIT_FAILURE)
		return (EXIT_FAILURE);

	std::string response;
	
	try {
		client.parsePacket(server);
		if (client.getStatus() == WAITING)
			return (EXIT_SUCCESS);
		response = MethodExecutor(server, client).getResponse().build();
	} catch (const RequestException& re) {
		std::cerr << re.what() << std::endl;
		response = Response("HTTP/1.1", server.getErrorPageByCode(re.getCode())).build();
	} catch (const std::exception& e) {
		std::cerr << "Unhandled exception (should never happen): " << e.what() << std::endl;
		response = Response("HTTP/1.1", server.getErrorPageByCode(INTERNAL_SERVER_ERROR)).build();
	}

	return (queueResponse(client, response, epollfd) == EXIT_FAILURE);
}

int createEpoll(int servfd) {
	int epollfd = epoll_create(1);
	if (epollfd == -1) {
		std::cerr << "epoll_create: " << strerror(errno) << std::endl;
		return (-1);
	}

	if (setNonBlocking(servfd) == EXIT_FAILURE) {
		close(epollfd);
		return (-1);
	}

	if (my_epoll_ctl(epollfd, EPOLL_CTL_ADD, EPOLLIN, servfd) == -1) {
		close(epollfd);
		return (-1);
	}
	return (epollfd);
}

void sigint_handler(int sig) {
	(void)sig;
	write(2, "\n", 1);
}

int launchEpoll(Server &server) {
	int servfd = createSocket(server);
	if (servfd == -1)
		return (EXIT_FAILURE);

	int epollfd = createEpoll(servfd);
	if (epollfd == -1) {
		close(servfd);
		return (EXIT_FAILURE);
	}

	struct epoll_event events[NB_EVENTS];
	int eventsReady;

	if (signal(SIGINT, &sigint_handler) == SIG_ERR) {
		std::cerr << "signal: " << strerror(errno) << std::endl;
		close(servfd);
		close(epollfd);
		return (EXIT_FAILURE);
	}
	
	while (1) {
		printf("epoll_waiting\n");
		eventsReady = epoll_wait(epollfd, events, MAX_EVENT_WAITED, -1);
		if (eventsReady == -1) {
			std::cerr << "epoll_wait: " << strerror(errno) << std::endl;
			close(servfd);
			close(epollfd);
			return (EXIT_FAILURE);
		}
		std::cout << eventsReady << " event(s)" << std::endl;
		for (int i = 0; i < eventsReady; ++i) {
			if (events[i].data.fd == servfd) {
				if (addClient(server, servfd, epollfd) == EXIT_FAILURE) {
					continue;
				}
			} else {
				Client& client = server.getClient(events[i].data.fd);
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
	close(servfd);
	return 0;
}
