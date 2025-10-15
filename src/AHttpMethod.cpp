/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 15:39:10 by dorianmazar       #+#    #+#             */
/*   Updated: 2025/10/15 16:00:19 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHttpMethod.hpp"
#include <unistd.h>
#include <string>


std::string	readPage(std::string fileName)
{
	std::string body;
	std::ifstream stream(fileName.c_str());
	std::string line;

	while (std::getline(stream, line))
	{
		body += line;
		body += "\n\r";
	}
	body += "\n\r";
	return (body);
}

Response AHttpMethod::GET(std::string fileName, Location loc, Request req)
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
				return  res;
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
			return  res;
		}
		else
			; // return ErrorPage();
	}
	Response	error(req.getVersion(), 404, "Not Found", "");
	return error;
}
