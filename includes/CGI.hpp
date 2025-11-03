/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 13:51:52 by mazakov           #+#    #+#             */
/*   Updated: 2025/11/04 00:05:54 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "Context.hpp"
#include <time.h>

class CGI {
	private:
		Server		&_server;
		Client		&_client;
		int			_pid;
		int			_fd;
		int			_startTime;
		int			_timeOut;
		std::string	_output;

	public:
		CGI(Server &, Client &);
		CGI(const CGI &);
		~CGI();

		Client		&getClient();
		Server		&getServer();
		int			getClientFd();
		int			getPid();
		int			getFd();
		int			getStartTime();
		int			getTimeOutValue();
		std::string	&getOutput();

		void	setPid(int);
		void	setFd(int);
		void	appendOutput(const std::string &);
};

#endif