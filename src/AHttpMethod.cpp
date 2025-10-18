/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/10/18 16:10:15 by faoriol          ###   ########.fr       */
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
	return body;
}

Response AHttpMethod::GET(std::string fileName, Location& loc, Request& req, Server& serv)
{
	std::vector<std::string> vec = loc.getIndex();
	std::string index;
	
	if (fileName[fileName.size() - 1] == '/')
	{
		for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		{
			std::string tmp(fileName + *it);
			if (access(tmp.c_str(), R_OK) == 0)
				return Response(req.getVersion(), 200, "OK", readPage(tmp));
		}
		if (loc.getAutoIndex() == true)
			; // return LoadAutoIndex();
		else
			return Response(req.getVersion(), serv.getErrorPageByCode(404));
	}
	else
	{
		if (access(fileName.c_str(), R_OK) == 0)
			return Response(req.getVersion(), 200, "OK", readPage(fileName));
		else
			return Response(req.getVersion(), serv.getErrorPageByCode(404));
	}
	return Response(req.getVersion(), serv.getErrorPageByCode(404));
}

Response	AHttpMethod::DELETE(std::string filename, Request& req, Server& serv)
{
	std::string directory = filename.substr(0, filename.find_last_of("/") + 1);
	if (access(directory.c_str(), W_OK) != 0)
		return Response(req.getVersion(), serv.getErrorPageByCode(403));
	if (std::remove(filename.c_str()) != 0)
		return Response(req.getVersion(), serv.getErrorPageByCode(404));
	return Response(req.getVersion(), 200, "OK", "");
}
