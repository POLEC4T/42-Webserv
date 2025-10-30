/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 16:47:21 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/30 16:44:31 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"
#include "Server.hpp"
#include "epoll.hpp"

int main(int ac, char **av) {
	Context ctx;

	if (ac > 2)
	{
		std::cerr << "Usage: ./webserv [ ConfigFile ( optional ) ]" << std::endl;
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
	
	if (ac == 2)
	{
		if (!FtString(std::string(av[1])).endsWith(".conf")) {
			std::cerr << "Usage: configuration file need .conf" << std::endl;
			return 1;
		}
		try {
			ctx.configFileParser(av[1], ctx.getMapDefaultErrorPage());
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}
	if (ac == 1)
	{
		try {
			ctx.configFileParser("configs/default.conf", ctx.getMapDefaultErrorPage());
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}

	std::vector<Server> servers = ctx.getServers();
	std::vector<Server>::iterator it;
	for (it = servers.begin(); it != servers.end(); it++)
		it->setDefaultMapErrorPage(ctx.getMapDefaultErrorPage());

	return (launchEpoll(ctx) == EXIT_FAILURE);
}