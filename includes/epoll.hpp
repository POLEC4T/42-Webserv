/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 10:11:47 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/30 11:05:40 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
#define EPOLL_HPP

#include "CodeDefines.h"
#include "Headers.h"
#include "MethodExecutor.hpp"
#include "Request.hpp"
#include "Server.hpp"

int launchEpoll(Server &server);
int my_epoll_ctl(int epollfd, int op, uint32_t events, int fd);

#endif