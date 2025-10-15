/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/10/15 16:58:26 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHttpMethod.hpp"
#include <unistd.h>
#include "Server.hpp"
#include <string>

std::string	readPage(std::string fileName)
{
	std::string body;
	std::ifstream stream(fileName.c_str());
	std::string line;

	while (std::getline(stream, line))
	{
		body += line;
		body += "\r\n";
	}
	body += "\r\n";
	return (body);
}

Response AHttpMethod::GET(std::string fileName, Location loc, Request req, Server serv)
{
	std::vector<std::string> vec = loc.getIndex();
	std::string index;
	
	if (fileName[fileName.size() - 1] == '/')
	{
		for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		{
			std::string tmp(fileName + *it);
			if (access(tmp.c_str(), R_OK) == 0)
			{
				Response	res(req.getVersion(), 200, "OK", readPage(tmp));
				res.setHeader("Content-Length", FtString::my_to_string(res.getBody().size()));
				res.setHeader("Content-Type", "text/html");
				return res;
			}
		}
		if (loc.getAutoIndex() == true)
			; // return LoadAutoIndex();
		else
			; // return ErrorPage();
	}
	else
	{
		if (access(fileName.c_str(), R_OK) == 0)
		{
			Response	res(req.getVersion(), 200, "OK", readPage(fileName));
			res.setHeader("Content-Length", FtString::my_to_string(res.getBody().size()));
			res.setHeader("Content-Type", "text/html");
			return  res;
		}
		else
		{
			ErrorPage	page(serv.getErrorPageByCode(404));
			std::cout << "OEQWEQEQEQE" << page.getContent() << std::endl;
			Response res(req.getVersion(), page.getCode(), "Not Found", page.getContent());
			res.setHeader("Content-Length", FtString::my_to_string(page.getContent().size()));
			return res;

		}
	}
	ErrorPage	page(serv.getErrorPageByCode(404));
	std::cout << "OEQWEQEQEQE" << page.getContent() << std::endl;
	Response res(req.getVersion(), page.getCode(), "Not Found", page.getContent());
	res.setHeader("Content-Length", FtString::my_to_string(page.getContent().size()));
	return res;
}
