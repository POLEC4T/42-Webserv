/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 10:46:35 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/27 19:02:44 by faoriol          ###   ########.fr       */
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


int queueResponse(Client &client, std::string& response, int epollfd) {

	if (my_epoll_ctl(epollfd, EPOLL_CTL_MOD, EPOLLIN | EPOLLOUT | EPOLLET, client.getFd()) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	client.setSendBuffer(response);
	return(EXIT_SUCCESS);
}

int readRequest(Server& server, Client &client) {
	char buffer[BUFFER_SIZE] = { 0 };
	ssize_t sizeRead;

	sizeRead = recv(client.getFd(), buffer, BUFFER_SIZE, 0);
	if (sizeRead == -1) {
		std::cerr << "recv:" << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	} else if (sizeRead == 0) {
		std::cerr << "Client disconnected" << std::endl;
		server.deleteClient(client.getFd());
		return (EXIT_FAILURE);
	}
	client.appendBuffer(buffer);
	return (EXIT_SUCCESS);
}

/**
 * - Sets up a TCP server socket
 * - Binds it to the specified address and port
 * - Listens for incoming connections
 * STRESS TESTED
 */
int createSocket(const Server &server) {
  int servfd;
  struct addrinfo *addrinfos;
  struct addrinfo hints;

  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // ipv4 pelo
  hints.ai_socktype = SOCK_STREAM; // tcp
  hints.ai_flags = AI_PASSIVE;     // for bind()

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
    std::cerr << "fcntl: " << strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * erreurs possibles :
 * 		accept crash -> pas possible car servfd est dit grace a epoll, mais si ca marche pas webserv doit exit
 * 		setNonBlocking (= fctnl) crash -> webserv doit exit
 * 		epoll_ctl(ADD) crash -> webserv doit exit
 * 	-> dans tous les cas webserv doit exit
 */
int addClient(Server &server, int servfd, int epollfd) {
  struct sockaddr clientAddr;
  int addrLen = sizeof(clientAddr);
  int clientfd =
      accept(servfd, (struct sockaddr *)&clientAddr, (socklen_t *)&(addrLen));
  if (clientfd == -1) {
    std::cerr << "accept: " << strerror(errno) << std::endl;
    return (EXIT_FAILURE);
  }

	if (setNonBlocking(clientfd) == EXIT_FAILURE) {
		close(clientfd);
		return (EXIT_FAILURE);
	}
	
	if (my_epoll_ctl(epollfd, EPOLL_CTL_ADD, EPOLLIN | EPOLLET, clientfd) == -1) {
		return (EXIT_FAILURE);
	}
	
	Client cl(clientfd);
	server.addClient(cl);
	std::cout << "New connection, fd: " << clientfd << std::endl;
	return (EXIT_SUCCESS);
}

int handleClientIn(Server& server, Client& client, int epollfd) {

	if (readRequest(server, client) == EXIT_FAILURE)
		return (EXIT_FAILURE);

	std::string response;
	try {
		client.parseRequest(server);
		if (client.getStatus() == WAITING)
			return (EXIT_SUCCESS);
		response = MethodExecutor(server, client).execute();
	}  catch (const RequestException& re) {
		std::cout << re.what() << std::endl;
		response = Response("HTTP/1.1", server.getErrorPageByCode(re.getCode())).build();
	} catch (const std::exception& e) {
		std::cerr << "Unhandled exception: " << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

	if (queueResponse(client, response, epollfd) == EXIT_FAILURE) {
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
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

	if (my_epoll_ctl(epollfd, EPOLL_CTL_ADD, EPOLLIN | EPOLLET, servfd) == -1) {
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
					close(servfd);
					close(epollfd);
					return (EXIT_FAILURE);
				}
			} else {
				Client& client = server.getClient(events[i].data.fd);
				if (events[i].events & EPOLLIN) {
					if (handleClientIn(server, client, epollfd) == EXIT_FAILURE)
						continue;
				}
				if (events[i].events & EPOLLOUT) {
					std::cout << "Sending response to client fd: " << client.getFd() << std::endl;
					if (client.sendPendingResponse(epollfd) == EXIT_FAILURE)
						continue;
				}
			}
		}
		std::cout << "-----\n";
	}
	close(servfd);
	return 0;
}
