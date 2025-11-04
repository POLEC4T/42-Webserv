/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 13:51:52 by mazakov           #+#    #+#             */
/*   Updated: 2025/11/04 12:49:08 by dmazari          ###   ########.fr       */
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
		Request		&_request;
		int			_pid;
		int			_fd;
		int			_startTime;
		int			_timeOut;
		std::string	_output;

	public:
		CGI(Server &, Client &, Request&);
		CGI(const CGI &);
		~CGI();

		Client		&getClient();
		Server		&getServer();
		Request		&getRequest();
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