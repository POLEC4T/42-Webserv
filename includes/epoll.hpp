/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 10:11:47 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/22 15:57:46 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
#define EPOLL_HPP

# include <sys/socket.h>
# include <sys/types.h>
# include <sys/epoll.h>
# include <netdb.h>
# include <iostream>
# include <unistd.h>
# include <sstream>
# include <fstream>
# include <iostream>
# include <exception>
# include <cstdlib>
# include <fcntl.h>
# include <cstring>
# include <signal.h>
# include <errno.h>
# include "Server.hpp"
# include "Request.hpp"
# include "Client.hpp"
# include "MethodExecutor.hpp"
# include "CodeDefines.h"

int	launchEpoll(Server &server);
int my_epoll_ctl(int epollfd, int op, uint32_t events, int fd);

#endif