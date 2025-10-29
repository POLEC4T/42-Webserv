/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 16:47:21 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/29 11:43:53 by mniemaz          ###   ########.fr       */
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

	if (launchEpoll(ctx) == -1)
	{
		return (1);
	}
	return 0;
}



// // #include <limits>


// int main() {
// 	std::string firstpart = 	"3";

// 	std::string secondpart = 	"3\r\n"
// 								"01";

// 	std::string lastpart = 	"3\r\n"
// 							"012\r\n"
// 							"0\r\n"
// 							"\r\n";

								

// 	Client client;
	
// 	bool fullyReceived;
// 	////
// 	try {
// 		fullyReceived = client.unchunkBody(firstpart);
// 	} catch (const RequestException& e) {
// 		std::cerr << e.what() << std::endl;
// 	}
// 	if (fullyReceived)
// 		std::cout << "unchunked: '" << client.getRequest().getBody() << "'" << std::endl;
// 	else
// 		std::cout << "Not fully received" << std::endl;

// 	////
// 	try {
// 		fullyReceived = client.unchunkBody(secondpart);
// 	} catch (const RequestException& e) {
// 		std::cerr << e.what() << std::endl;
// 	}
// 	if (fullyReceived)
// 		std::cout << "unchunked: '" << client.getRequest().getBody() << "'" << std::endl;
// 	else
// 		std::cout << "Not fully received" << std::endl;

// 	////
// 	try {
// 		fullyReceived = client.unchunkBody(lastpart);
// 	} catch (const RequestException& e) {
// 		std::cerr << e.what() << std::endl;
// 	}
// 	if (fullyReceived)
// 		std::cout << "unchunked: '" << client.getRequest().getBody() << "'" << std::endl;
// 	else
// 		std::cout << "Not fully received" << std::endl;



	
// 	return (0);
// }