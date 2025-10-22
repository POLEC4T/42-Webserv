/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 16:47:21 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/22 17:09:12 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"
#include "Headers.h"
#include "Server.hpp"
#include "epoll.hpp"

int main(int ac, char **av) {
  Context ctx;

	if (ac != 2)
	{
		std::cerr << "Usage: ./webserv [ConfigFile]" << std::endl;
		return 1;
	}
	try {
		ctx.parseAndSetMapDefaultErrorPage();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	std::map<int, ErrorPage> errorPages = ctx.getMapDefaultErrorPage();
	if (ac != 2)
	{
		std::cerr << "Usage: ./webserv [ConfigFile]" << std::endl;
		return 1;
	}
	try {
		ctx.configFileParser(av[1], ctx.getMapDefaultErrorPage());
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

  std::vector<Server> servers = ctx.getServers();
  for (std::vector<Server>::iterator it = servers.begin(); it != servers.end();
       it++)
    it->setDefaultMapErrorPage(ctx.getMapDefaultErrorPage());

  Server &serv = servers[0];

  if (launchEpoll(serv) == -1) {
    return (1);
  }
  return 0;
}