/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 13:51:52 by mazakov           #+#    #+#             */
/*   Updated: 2025/11/03 11:01:48 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "Context.hpp"
#include <time.h>

class CGI {
	private:
		Server&			_server;
		Client&			_client;
		int				_pid;
		int				_fd;
		int				_startTime;
		int				_timeOut;
		std::string		_output;
	public:
		CGI(Server&, Client&);
		CGI(const CGI&);
		~CGI();

		Client&	getClient();
		int	getClientFd();
		Server&	getServer();
		int	getPid();
		int	getFd();
		int	getStartTime();
		int	getTimeOutValue();
		std::string&	getOutput();

		void	setPid(int);
		void	setFd(int);
		void	appendOutput(const std::string&);
	};

#endif