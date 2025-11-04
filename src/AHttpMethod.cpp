/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/11/04 14:41:51 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHttpMethod.hpp"
#include "CodeDefines.h"
#include "Server.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

long long getMaxBodySize(Location &loc, Server &serv) {
	long long defaultV = 1048576;
	if (loc.getClientMaxBodySize() != -1)
		return loc.getClientMaxBodySize();
	if (serv.getClientMaxBodySize() != -1)
		return serv.getClientMaxBodySize();
	return defaultV;
}

std::string readPage(std::string fileName) {
	std::string body;
	std::ifstream stream(fileName.c_str(), std::ios::in | std::ios::binary);
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

Response LoadAutoIndex(Request &req, std::string &path, Server &serv,
					std::string root) {
	std::string page = "<!DOCTYPE html><html lang=\"fr\"><head><meta "
					"charset=\"UTF-8\"><title>AutoIndex</title>"
					"<style>"
					"body {"
					"font-family: 'Inter', 'Segoe UI', Roboto, sans-serif;"
					"background-color: #202125;"
					"color: #e6e6e6;"
					"margin: 0;"
					"padding: 50px 0;"
					"display: flex;"
					"flex-direction: column;"
					"align-items: center;"
					"}"
					"h1 {"
					"color: #fafafa;"
					"margin-bottom: 40px;"
					"font-weight: 500;"
					"font-size: 1.6em;"
					"letter-spacing: 0.5px;"
					"}"
					".file-list {"
					"width: 90%;"
					"max-width: 800px;"
					"border-top: 1px solid #333;"
					"}"
					".file-row {"
					"display: flex;"
					"justify-content: space-between;"
					"align-items: center;"
					"padding: 12px 10px;"
					"border-bottom: 1px solid #333;"
					"transition: background 0.2s;"
					"}"
					".file-row:hover {"
					"background-color: #2a2b30;"
					"}"
					".file-info {"
					"display: flex;"
					"align-items: center;"
					"gap: 8px;"
					"font-size: 0.95em;"
					"}"
					".file-name {"
					"color: #f0f0f0;"
					"text-decoration: none;"
					"}"
					".file-name:hover {"
					"color: #ffffff;"
					"}"
					".file-type {"
					"opacity: 0.6;"
					"font-size: 0.85em;"
					"}"
					".buttons {"
					"display: flex;"
					"gap: 8px;"
					"}"
					".btn {"
					"padding: 5px 12px;"
					"border-radius: 4px;"
					"font-size: 0.85em;"
					"text-decoration: none;"
					"font-weight: 500;"
					"transition: all 0.2s ease;"
					"border: 1px solid transparent;"
					"}"
					".btn:hover {"
					"transform: translateY(-1px);"
					"}"
					".open-btn {"
					"background-color: #ffffff;"
					"color: #000;"
					"border: 1px solid #ddd;"
					"}"
					".open-btn:hover {"
					"background-color: #f0f0f0;"
					"}"
					".download-btn {"
					"background-color: #e53935;"
					"color: #fff;"
					"}"
					".download-btn:hover {"
					"background-color: #ef5350;"
					"}"
					"</style>"
					"</head><body><h1>INDEX</h1><div class=\"file-list\">";

	DIR *directory = opendir(path.c_str());
	if (directory == NULL)
		return Response(req.getVersion(),
						serv.getErrorPageByCode(PAGE_NOT_FOUND));

	struct dirent *list;
	while ((list = readdir(directory)) != NULL) {
		if (!strcmp(list->d_name, "."))
			continue;

		int pos = path.find(root);
		std::string relative_path =
			path.substr(pos + root.size(), path.size() - (pos + root.size()));
		if (relative_path.size() != 1)
			relative_path += "/";

		page += "<div class=\"file-row\">";
		page += "<div class=\"file-info\">";
		if (list->d_type == DT_DIR)
			page += "<span class=\"file-type\">📁</span>";
		else
			page += "<span class=\"file-type\">📄</span>";
		page += "<a class=\"file-name\" href=\"" + relative_path +
				list->d_name + "\">" + std::string(list->d_name);
		if (list->d_type == DT_DIR)
			page += "/";
		page += "</a></div><div class=\"buttons\">";
		if (list->d_type != DT_DIR)
			page += "<a class=\"btn download-btn\" href=\"" + relative_path +
					list->d_name + "\" download>Télécharger</a>";
		page += "<a class=\"btn open-btn\" href=\"" + relative_path +
				list->d_name + "\">Ouvrir</a>";
		page += "</div></div>";
	}

	page += "</div></body></html>";
	closedir(directory);
	return Response(req.getVersion(), OK, "OK", page);
}

Response AHttpMethod::GET(std::string fileName, Location &loc, Request &req,
						Server &serv) {
	std::vector<std::string> vec = loc.getIndex();
	std::string index;
	struct stat infos;

	if (fileName[fileName.size() - 1] == '/') {
		for (std::vector<std::string>::iterator it = vec.begin();
			it != vec.end(); it++) {
			std::string tmp(fileName + *it);
			if (access(tmp.c_str(), R_OK) == 0)
				return Response(req.getVersion(), OK, "OK", readPage(tmp));
		}
		if (loc.getAutoIndex() == true)
			return LoadAutoIndex(req, fileName, serv, loc.getRoot());
		else
			return Response(req.getVersion(),
							serv.getErrorPageByCode(PAGE_NOT_FOUND));
	} else {
		if (stat(fileName.c_str(), &infos) != 0)
			return Response(req.getVersion(),
							serv.getErrorPageByCode(PAGE_NOT_FOUND));
		if (loc.getAutoIndex() == true && S_ISDIR(infos.st_mode))
			return LoadAutoIndex(req, fileName, serv, loc.getRoot());
		if (access(fileName.c_str(), R_OK) == 0)
			return Response(req.getVersion(), OK, "OK", readPage(fileName));
		else
			return Response(req.getVersion(),
							serv.getErrorPageByCode(PAGE_NOT_FOUND));
	}
	return Response(req.getVersion(), serv.getErrorPageByCode(PAGE_NOT_FOUND));
}

Response	AHttpMethod::DELETE(std::string filename, Request& req, Server& serv)
{
    if (FtString(filename).endsWith("/"))
        return Response(req.getVersion(), serv.getErrorPageByCode(FORBIDDEN));
    std::string directory = filename.substr(0, filename.find_last_of("/") + 1);
	if (access(directory.c_str(), W_OK) != 0)
        return Response(req.getVersion(), serv.getErrorPageByCode(FORBIDDEN));

	if (std::remove(filename.c_str()) != 0)
		return Response(req.getVersion(),
						serv.getErrorPageByCode(PAGE_NOT_FOUND));

	return Response(req.getVersion(), OK, "OK", "");
}

Response AHttpMethod::POST(std::string filename, Request& req, Server& serv)
{
    struct stat* infos = new struct stat;
	std::memset(infos, 0, sizeof(struct stat));
	
    if (stat(filename.c_str(), infos) != 0)
    {
        if (infos && S_ISDIR(infos->st_mode))
        {
            delete infos;
            return Response(req.getVersion(), serv.getErrorPageByCode(FORBIDDEN));
        }
    }
    delete infos;
	std::string directory = filename.substr(0, filename.find_last_of("/") + 1);
	if (access(directory.c_str(), W_OK) != 0)
		return Response(req.getVersion(), serv.getErrorPageByCode(FORBIDDEN));

	std::ofstream file(filename.c_str(), std::ostream::out);
	if (!file.is_open())
		return Response(req.getVersion(),
						serv.getErrorPageByCode(PAGE_NOT_FOUND));
	file << req.getBody();

	return Response(req.getVersion(), OK, "OK", "");
}