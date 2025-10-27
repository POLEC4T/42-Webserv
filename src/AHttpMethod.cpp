/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/10/27 15:02:47 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHttpMethod.hpp"
#include <unistd.h>
#include "Server.hpp"
#include <string>
#include <dirent.h>
#include "CodeDefines.h"
#include <sys/stat.h>

long long getMaxBodySize(Location& loc, Server& serv)
{
	long long	defaultV = 1048576;
	if (loc.getClientMaxBodySize() != -1)
		return loc.getClientMaxBodySize();
	if (serv.getClientMaxBodySize() != -1)
		return serv.getClientMaxBodySize();
	return defaultV;
}

std::string	readPage(std::string fileName)
{
	std::string body;
	std::ifstream stream(fileName.c_str() , std::ios::in | std::ios::binary);
	if (!stream)
		return "";
	stream.seekg(0, std::ios::end);
	std::streampos size = stream.tellg();
	stream.seekg(0, std::ios::beg);
	
	body.resize(static_cast<size_t>(size));
	
	if (size > 0)
        stream.read(&body[0], size);
	return body;
}

Response	LoadAutoIndex(Request& req, std::string& path, Server& serv, std::string root)
{
	std::string page = "<!DOCTYPE html><html lang=\"fr\"><head><meta charset=\"UTF-8\"><title>auto-index</title><style>body {font-family: Arial, sans-serif;background-color: #f4f4f4;padding: 30px;}h1 {color: #333;}.file-list {margin-top: 20px;}.file-list a {display: block;text-decoration: none;color: #1a73e8;padding: 4px 12px;margin-bottom: 5px;border-radius: 4px;background-color: #ffffff;transition: background-color 0.2s;}.file-lista:hover{background-color: #e8f0fe;}.directory{font-weight:bold;color: #2e7d32;}.file{color: #1a237e;}</style></head><body><h1>auto-index</h1><div class=\"file-list\">";
	DIR* directory = opendir(path.c_str());
	if (directory == NULL)
		return Response(req.getVersion(), serv.getErrorPageByCode(PAGE_NOT_FOUND));
	struct dirent* list;
	while ((list = readdir(directory)) != NULL)
	{
		if (!strcmp(list->d_name, "..") || !strcmp(list->d_name, "."))
			continue;
		int pos = path.find(root);
		page += "<a href=\"";
		std::string tmp = path.substr(pos + root.size(), path.size() - (pos + root.size()));
		if (tmp.size() != 1)
			page += tmp + "/";
		page += list->d_name;
		page += "\" download>";
		page += list->d_name;
		if (list->d_type == DT_DIR)
			page += "/";
		page += "</a><br>";
	}
	page += "</div></body></html>";
	closedir(directory);
	return Response(req.getVersion(), OK, "OK", page);
}

Response AHttpMethod::GET(std::string fileName, Location& loc, Request& req, Server& serv)
{
	std::vector<std::string> vec = loc.getIndex();
	std::string index;
	struct	stat infos;
	
	if (fileName[fileName.size() - 1] == '/')
	{
		for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		{
			std::string tmp(fileName + *it);
			if (access(tmp.c_str(), R_OK) == 0)
				return Response(req.getVersion(), OK, "OK", readPage(tmp));
		}
		if (loc.getAutoIndex() == true)
			return LoadAutoIndex(req, fileName, serv, loc.getRoot());
		else
			return Response(req.getVersion(), serv.getErrorPageByCode(PAGE_NOT_FOUND));
	}
	else
	{
		if (stat(fileName.c_str(), &infos) != 0)
			return Response(req.getVersion(), serv.getErrorPageByCode(PAGE_NOT_FOUND));
		if (loc.getAutoIndex() == true && S_ISDIR(infos.st_mode))
			return LoadAutoIndex(req, fileName, serv, loc.getRoot());
		if (access(fileName.c_str(), R_OK) == 0)
			return Response(req.getVersion(), OK, "OK", readPage(fileName));
		else
			return Response(req.getVersion(), serv.getErrorPageByCode(PAGE_NOT_FOUND));
	}
	return Response(req.getVersion(), serv.getErrorPageByCode(PAGE_NOT_FOUND));
}

Response	AHttpMethod::DELETE(std::string filename, Request& req, Server& serv)
{
	std::string directory = filename.substr(0, filename.find_last_of("/") + 1);
	if (access(directory.c_str(), W_OK) != 0)
		return Response(req.getVersion(), serv.getErrorPageByCode(FORBIDDEN));

	if (std::remove(filename.c_str()) != 0)
		return Response(req.getVersion(), serv.getErrorPageByCode(PAGE_NOT_FOUND));
		
	return Response(req.getVersion(), OK, "OK", "");
}

Response AHttpMethod::POST(std::string filename, Request& req, Server& serv)
{
	std::string directory = filename.substr(0, filename.find_last_of("/") + 1);
	if (access(directory.c_str(), W_OK) != 0)
		return Response(req.getVersion(), serv.getErrorPageByCode(FORBIDDEN));
		
	std::ofstream	file(filename.c_str(), std::ostream::out);
	if (!file.is_open())
		return Response(req.getVersion(), serv.getErrorPageByCode(PAGE_NOT_FOUND));
	file << req.getBody();
	
	return Response(req.getVersion(), OK, "OK", "");
}