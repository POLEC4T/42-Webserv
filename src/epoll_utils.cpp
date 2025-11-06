/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll_utils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 18:16:32 by mniemaz           #+#    #+#             */
/*   Updated: 2025/11/06 10:49:43 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "epoll.hpp"

# include <sys/signal.h>
# include <iostream>
# include <sstream>
# include <unistd.h>
# include <errno.h>
# include <netdb.h>
# include <fcntl.h>
# include <cstring>
# include <cstdlib>

void sigint_handler(int sig) {
	(void)sig;
	write(2, "\n", 1);
}

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

const std::string printIP(uint32_t v_host) {
    uint32_t v_net = htonl(v_host);

    unsigned char b0 = (v_net >> 24)	& 0xFF;
    unsigned char b1 = (v_net >> 16)	& 0xFF;
    unsigned char b2 = (v_net >> 8)		& 0xFF;
    unsigned char b3 = (v_net)			& 0xFF;

    std::ostringstream oss;
    oss << int(b0) << '.' << int(b1) << '.' << int(b2) << '.' << int(b3);
    return oss.str();
}