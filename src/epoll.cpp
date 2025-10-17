/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 10:46:35 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/17 13:38:45 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

#define MAX_EVENT_WAITED 1024
#define NB_EVENTS 1024
#define BUFFER_SIZE 1024

int readRequest(Server& server, Client &client) {
	char buffer[BUFFER_SIZE] = { 0 };

	ssize_t sizeRead;
	sizeRead = recv(client.getFd(), buffer, BUFFER_SIZE, 0);
	if (sizeRead == -1) {
		std::cerr << "recv:" << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	} else if (sizeRead == 0) {
		std::cerr << "Client disconnected" << std::endl;
		server.removeClient(client.getFd());
		return (EXIT_FAILURE);
	}
	client.appendBuffer(buffer);
	
	if (client.hasReceivedFullReq())
		client.setStatus(READY);
	else
		client.setStatus(WAITING);
	return (EXIT_SUCCESS);
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

int setNonBlocking(int sockfd) {
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "fcntl" << strerror(errno) <<std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int addClient(Server& server, int servfd, int epollfd) {
	struct sockaddr clientAddr;
	int addrLen = sizeof(clientAddr);
	int clientfd = accept(servfd, (struct sockaddr *)&clientAddr, (socklen_t *)&(addrLen));
	if (clientfd == -1) {
		std::cerr << "accept:" << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}

	if (setNonBlocking(clientfd) == EXIT_FAILURE)
		return (EXIT_FAILURE);

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET; //EPOLLET: epoll_wait() will only report an event once
	ev.data.fd = clientfd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev) == -1) {
		std::cerr << "epoll_ctl: " << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}
	Client cl(clientfd);

	server.addClient(cl);
	std::cout << "New connection, fd: " << clientfd << std::endl;
	return (EXIT_SUCCESS);
}

int handleClient(Server& server, int clientfd) {
	Request req;
	Client& client = server.getClient(clientfd);
	
	if (readRequest(server, client) == EXIT_FAILURE)
		return (EXIT_FAILURE);

	if (client.getStatus() == WAITING)
		return (EXIT_SUCCESS);

	try {
		req.parseRequest(client.getBuffer());
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		client.clearBuffer();
		return (EXIT_FAILURE);
	}

	MethodExecutor me(server, req, req.getMethod());
	
	std::string response = me.getResponse().build();

	std::cout << "response: \n" << response << std::endl;

	client.clearBuffer();

	if (send(clientfd, response.c_str(), response.size(), 0) == -1)
		std::cout << "send: " << strerror(errno) << std::endl;

	std::cout << "Response sent" << std::endl;
	return (EXIT_SUCCESS);
}

int launchEpoll(Server &server) {
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
	if (setNonBlocking(servfd) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = servfd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servfd, &ev) == -1) {
		std::cerr << "epoll_ctl:" << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}

	int eventsReady;
	while (1) {
		printf("epoll_waiting\n");
		eventsReady = epoll_wait(epollfd, events, MAX_EVENT_WAITED, -1);
		if (eventsReady == -1) {
			std::cerr << "epoll_wait:" << strerror(errno) << std::endl;
			return (EXIT_FAILURE);
		}
		std::cout << eventsReady << " event(s)" << std::endl;
		for (int i = 0; i < eventsReady; ++i) {
			if (events[i].data.fd == servfd) {
				if (addClient(server, servfd, epollfd) == EXIT_FAILURE)
					return (EXIT_FAILURE);
			} else {
				if (handleClient(server, events[i].data.fd) == EXIT_FAILURE)
					continue;
			}
		}
		std::cout << "-----\n";
	}
	close(servfd);
	return 0;
}

// todo: une requete peut arriver en plusieurs morceaux -> faire un buffer par client qui persiste
// pareil pour send