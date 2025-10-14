/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 10:19:50 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/14 10:24:17 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"
#include "Request.hpp"
#include "epoll.hpp"

int main(int ac, char **av) {
	Context	ctx;

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
	// for (std::map<int, ErrorPage>::iterator it = errorPages.begin(); it != errorPages.end(); it++) {
	// 	std::cout << "Name: " << it->second.getName() << std::endl;
	// 	std::cout << "Content: " << it->second.getContent() << std::endl;
	// 	std::cout << "Code: " << it->second.getCode() << std::endl;
	// }
	
	
	try {
		ctx.configFileParser(av[1], errorPages);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	// std::string reqExample;
	// reqExample += "GET / HTTP/1.1\r\n";
	// reqExample += "Host: localhost\r\n";
	// reqExample += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\r\n";
	// reqExample += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
	// reqExample += "Accept-Language: en-US,en;q=0.5\r\n";
	// reqExample += "Accept-Encoding: gzip, deflate\r\n";
	// reqExample += "Connection: keep-alive\r\n";
	// reqExample += "Cache-Control: max-age=0\r\n";
	// reqExample += "\r\n";

	const std::vector<Server> servers = ctx.getServers();
	Server serv = servers[0];

	int ret = launchEpoll(serv);
	if (ret == EXIT_FAILURE) {
		return (EXIT_FAILURE);
	}
	
	// Request req(serv);
	// try {
	// 	req.parseRequest(reqExample);
	// } catch (std::exception &e) {
		
	// 	std::cout << e.what() << std::endl;
	// }

	// // req.displayRequest();

	return 0;
}