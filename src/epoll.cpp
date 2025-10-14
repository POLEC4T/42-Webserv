/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 10:46:35 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/14 15:50:14 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

#define MAX_EVENT_WAITED 1024
#define NB_EVENTS 1024
#define BUFFER_SIZE 1024

void readRequest(int clientFd, std::string &requestContent) {
	// todo : read while end != /r/n/r/n
	char buffer[BUFFER_SIZE] = { 0 };
	int limit = 0;

	ssize_t sizeRead;
	while (1) {
		if (limit > 10)
		{
			std::cerr << "limit reached " << std::endl;
			break;
		}
		limit++;
		sizeRead = recv(clientFd, buffer, BUFFER_SIZE, 0);
		if (sizeRead == -1) {
			std::cerr << "recv:" << strerror(errno) << std::endl;
			break;
		} else if (sizeRead == 0) {
			std::cerr << "Client disconnected" << std::endl;
			close(clientFd);
			break;
		}

		requestContent.append(buffer, sizeRead);
		if (requestContent.find("\r\n\r\n") != std::string::npos)
			break;
	}
	// std::cout << requestContent << std::endl;
}

/**
 * - Sets up a TCP server socket
 * - Binds it to the specified address and port
 * - Listens for incoming connections
 */
int createSocket(const Server &server) {
	int servfd;
	
	struct addrinfo *addrinfos;
	struct addrinfo hints;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // ipv4 pelo
	hints.ai_socktype = SOCK_STREAM; // tcp
	hints.ai_flags = AI_PASSIVE; // for bind()

	int ret = getaddrinfo(server.getHost().c_str(),
						  server.getPort().c_str(),
						  &hints,
						  &addrinfos);
	if (ret != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(ret) << std::endl; // todo: test this
		return (-1);
	}

	servfd = socket(addrinfos->ai_family, addrinfos->ai_socktype, addrinfos->ai_protocol);
	if (servfd == -1) {
		std::cerr << "createSocket:" << strerror(errno) << std::endl;
		return (-1);
	}
	int opt = 1;
	setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	if (bind(servfd, addrinfos->ai_addr, addrinfos->ai_addrlen) == -1) {
		std::cerr << "bind: " << strerror(errno) << std::endl;
		return (-1);
	}
	if (listen(servfd, 2) == -1) {
		std::cerr << "listen: " << strerror(errno) << std::endl;
		return (-1);
	}
	freeaddrinfo(addrinfos);
	return (servfd);
}


const std::string& getTemplateResponse(const std::string &requestContent) {
	(void)requestContent;
	static const std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 95\r\n"
		"\r\n"
		"<!DOCTYPE html><html><head><title>Test</title></head>"
		"<body><h1>Hello, World!</h1></body></html>\r\n";
	return response;
}

/**
 * F_GETFL is FORBIDDEN
 */
int setNonBlocking(int sockfd) {
	int flags = fcntl(sockfd, F_GETFL, 0); // change this line to not use F_GETFL
	if (flags == -1) {
		std::cerr << "fcntl(F_GETFL)" <<std::endl;
		return -1;
	}
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "fcntl(F_SETFL)" <<std::endl;
		return -1;
	}
	return 0;
}

int launchEpoll(const Server &server) {
	int servfd = createSocket(server);
	if (servfd == -1)
		return 1;

	struct epoll_event ev;
	struct epoll_event events[NB_EVENTS];

	int epollfd = epoll_create(1);
	if (epollfd == -1) {
		std::cerr << "epoll_create:" << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}
	setNonBlocking(servfd);
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = servfd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servfd, &ev) == -1) {
		std::cerr << "epoll_ctl:" << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}

	int eventsReady;
	Request req(server);
	while (1) {
		printf("epoll_waiting\n");
		eventsReady = epoll_wait(epollfd, events, MAX_EVENT_WAITED, -1);
		if (eventsReady == -1) {
			std::cerr << "epoll_wait:" << strerror(errno) << std::endl;
			return (EXIT_FAILURE);
		} 
		std::cout << eventsReady << " event" << (eventsReady > 1 ? "s" : "") << " ready" << std::endl;
		for (int i = 0; i < eventsReady; ++i) {
			if (events[i].data.fd == servfd) {
				struct sockaddr clientAddr;
				int addrLen = sizeof(clientAddr);
				int clientFd = accept(servfd, (struct sockaddr *)&clientAddr, (socklen_t *)&(addrLen));
				if (clientFd == -1) {
					std::cerr << "accept:" << strerror(errno) << std::endl;
					return (EXIT_FAILURE);
				}

				setNonBlocking(clientFd);

				ev.events = EPOLLIN | EPOLLET; //EPOLLET: epoll_wait() will only report an event once
				ev.data.fd = clientFd;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
					std::cerr << "epoll_ctl: " << strerror(errno) << std::endl;
					return (EXIT_FAILURE);
				}
				std::cout << "New connection, fd: " << clientFd << std::endl;
			} else {
				std::string response;
				int clientFd = events[i].data.fd;
				try {
					std::string requestContent;
					readRequest(clientFd, requestContent);
					try {
						req.parseRequest(requestContent);
					} catch (const std::exception& e) {
						std::cerr << e.what() << std::endl;
						continue;
					}

					response = getTemplateResponse(requestContent);

					if (send(clientFd, response.c_str(), response.size(), 0) == -1)
						std::cout << "send: " << strerror(errno) << std::endl;

					std::cout << "Response sent" << std::endl;
				} catch (...) {
					std::cerr << "Error while creating response" << std::endl;
					close(events[i].data.fd);
					return (EXIT_FAILURE);
				}
			}
		}
		std::cout << "-----\n";
	}

	epoll_ctl(epollfd, EPOLL_CTL_DEL, servfd, NULL);
	close(servfd);

	
	return 0;
}

// todo: une requete peut arriver en plusieurs morceaux -> faire un buffer par client qui persiste
// pareil pour send