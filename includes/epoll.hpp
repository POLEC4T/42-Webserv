/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 10:11:47 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/22 17:09:20 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
#define EPOLL_HPP

#include "Headers.h"
# include "Server.hpp"
# include "Request.hpp"
# include "MethodExecutor.hpp"
# include "CodeDefines.h"

int	launchEpoll(Server &server);
int my_epoll_ctl(int epollfd, int op, uint32_t events, int fd);

#endif