/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 10:46:35 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/13 17:10:15 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <exception>
#include <sys/epoll.h>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

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
			std::cerr << "recv: error !" << std::endl;
			break;
		} else if (sizeRead == 0) {
			std::cerr << "Client disconnected" << std::endl;
			break;
		}

		requestContent.append(buffer, sizeRead);
		if (requestContent.find("\r\n\r\n") != std::string::npos)
			break;
	}
}

/**
 * - Sets up a TCP server socket
 * - Binds it to the specified address and port
 * - Listens for incoming connections
 */
void setupServer(int &servFd, struct sockaddr_in &servAddr){
	servFd = socket(AF_INET, SOCK_STREAM, 0);
	if (servFd == -1) {
		std::cerr << "socket err" << std::endl;
		throw std::exception();
	}
	int opt = 1;
	setsockopt(servFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(8080);
	servAddr.sin_addr.s_addr = INADDR_ANY;
		
	if (bind(servFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1) {
		std::cerr << "bind err" << std::endl;
		throw std::exception();
	}
		
	if (listen(servFd, 2) == -1) {
		std::cerr << "listen err" << std::endl;
		throw std::exception();
	}
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

int main(void) {
	struct sockaddr_in servAddr;
	int servFd;
	try {
		setupServer(servFd, servAddr);
	} catch (...) {
		return 1;
	}

	struct epoll_event ev;
	struct epoll_event events[NB_EVENTS];

	int epollFd = epoll_create(1);
	if (epollFd == -1) {
		std::cerr << "epoll_create err" << std::endl;
		return (1);
	}
	setNonBlocking(servFd);
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = servFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, servFd, &ev) == -1) {
		std::cerr << "epoll_ctl: servFd" << std::endl;
		return (1);
	}
		
	int eventsReady;
	while (1) {
		printf("Waiting to accept!\n");

		eventsReady = epoll_wait(epollFd, events, MAX_EVENT_WAITED, -1);
		if (eventsReady == -1) {
			std::cerr << "epoll_wait" << std::endl;
			return (1);
		} else {
			std::cout << eventsReady << " event" << (eventsReady > 1 ? "s" : "") << " ready" << std::endl;
		}
		for (int i = 0; i < eventsReady; ++i) {
			if (events[i].data.fd == servFd) {
				struct sockaddr clientAddr;
				int addrLen = sizeof(clientAddr);
				int clientFd = accept(servFd, (struct sockaddr *)&clientAddr, (socklen_t *)&(addrLen));
				if (clientFd == -1) {
					std::cerr << "accept" << std::endl;
					return (1);
				}

				setNonBlocking(clientFd);

				ev.events = EPOLLIN | EPOLLET; //EPOLLET: epoll_wait() will only report an event once
				ev.data.fd = clientFd;
				if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
					std::cerr << "epollctl: clientFd" << std::endl;
					return (1);
				}
				std::cout << "New connection, fd: " << clientFd << std::endl;
			} else {
				std::string response;
				int clientFd = events[i].data.fd;
				try {
					std::string requestContent;
					readRequest(clientFd, requestContent);
					response = getResponse(requestContent);

					if (send(clientFd, response.c_str(), response.size(), 0) == -1)
						std::cout << "send: error !" << std::endl;

					std::cout << "Response sent" << std::endl;
				} catch (...) {
					std::cerr << "Error while creating response" << std::endl;
					epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					close(events[i].data.fd);
					return (1) ;
				}
			}
		}
		std::cout << "-----\n";
	}

	epoll_ctl(epollFd, EPOLL_CTL_DEL, servFd, NULL);
	close(servFd);

	
	return 0;
}

// todo: une requete peut arriver en plusieurs morceaux -> faire un buffer par client qui persiste
// pareil pour send