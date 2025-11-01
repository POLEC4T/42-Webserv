/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodExecutor.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:25:48 by faoriol           #+#    #+#             */
/*   Updated: 2025/11/01 22:32:26 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __METHODEXECUTOR_HPP__
#define __METHODEXECUTOR_HPP__

#include "Client.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include <sys/types.h>

class MethodExecutor
{
	private:
		Server &_server;
		Client _client;
		std::string _method;
		Request _request;
		Response _response;

	public:
		MethodExecutor(Server &, Client &);
		std::string execute();
		Response &getResponse();
		static Location getRequestLocation(Request &, Server &);
		~MethodExecutor();
};

#endif
