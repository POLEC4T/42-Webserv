/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 10:11:47 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/30 11:17:37 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
#define EPOLL_HPP

class Context;
# include <sys/epoll.h>

# include <string>

int					launchEpoll(Context &ctx);
int					my_epoll_ctl(int epollfd, int op, uint32_t events, int fd);

void				sigint_handler(int sig);
int					setNonBlocking(int fd);
int					my_epoll_ctl(int epollfd, int op, uint32_t events, int fd);
const std::string	printIP(uint32_t v_host);

#endif