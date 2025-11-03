/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 16:47:21 by faoriol           #+#    #+#             */
/*   Updated: 2025/11/01 22:24:04 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"
#include "Server.hpp"
#include "epoll.hpp"

int main(int ac, char **av)
{
	Context ctx;

	if (ac > 2) {
		std::cerr << "Usage: ./webserv [ ConfigFile < .conf > ]" << std::endl;
		return 1;
	}
	try {
		ctx.parseAndSetMapDefaultErrorPage();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	std::map<int, ErrorPage> errorPages = ctx.getMapDefaultErrorPage();
	std::string config("configs/default.conf");

	if (ac == 2)
		config = av[1];
	if (!FtString(std::string(config)).endsWith(".conf")) {
		std::cerr << "Error: Wrong configuration File Extension" << std::endl;
		return 1;
	}
	try {
		ctx.configFileParser(config, ctx.getMapDefaultErrorPage());
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	std::vector<Server> servers = ctx.getServers();
	std::vector<Server>::iterator it;
	for (it = servers.begin(); it != servers.end(); it++)
		it->setDefaultMapErrorPage(ctx.getMapDefaultErrorPage());

	return (launchEpoll(ctx) == EXIT_FAILURE);
}
