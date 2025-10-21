/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dorianmazari <dorianmazari@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/10/20 13:41:49 by dorianmazar      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHttpMethod.hpp"
#include "Server.hpp"
#include <dirent.h>
#include <string>
#include <unistd.h>

std::string readPage(std::string fileName) {
  std::string body;
  std::ifstream stream(fileName.c_str());
  std::string line;

  while (std::getline(stream, line)) {
    body += line;
    body += "\r\n";
  }
  body += "\r\n";
  return body;
}

Response LoadAutoIndex(Request &req, Location &loc, Server &serv) {
  std::string page =
      "<!DOCTYPE html><html lang=\"fr\"><head><meta "
      "charset=\"UTF-8\"><title>auto-index</title><style>body {font-family: "
      "Arial, sans-serif;background-color: #f4f4f4;padding: 30px;}h1 {color: "
      "#333;}.file-list {margin-top: 20px;}.file-list a {display: "
      "block;text-decoration: none;color: #1a73e8;padding: 4px "
      "12px;margin-bottom: 5px;border-radius: 4px;background-color: "
      "#ffffff;transition: background-color "
      "0.2s;}.file-lista:hover{background-color: "
      "#e8f0fe;}.directory{font-weight:bold;color: #2e7d32;}.file{color: "
      "#1a237e;}</style></head><body><h1>auto-index</h1><div "
      "class=\"file-list\">";
  DIR *directory = opendir(loc.getRoot().c_str());
  if (directory == NULL)
    return Response(req.getVersion(), serv.getErrorPageByCode(404));
  struct dirent *list;
  while ((list = readdir(directory)) != NULL) {
    page += "<a href=\"";
    page += list->d_name;
    page += "\">";
    page += list->d_name;
    page += "</a><br>";
  }
  page += "</div></body></html>";
  closedir(directory);
  return Response(req.getVersion(), 200, "OK", page);
}

Response AHttpMethod::GET(std::string fileName, Location &loc, Request &req,
                          Server &serv) {
  std::vector<std::string> vec = loc.getIndex();
  std::string index;

  if (fileName[fileName.size() - 1] == '/') {
    for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end();
         it++) {
      std::string tmp(fileName + *it);
      if (access(tmp.c_str(), R_OK) == 0)
        return Response(req.getVersion(), 200, "OK", readPage(tmp));
    }
    if (loc.getAutoIndex() == true)
      return LoadAutoIndex(req, loc, serv);
    else
      return Response(req.getVersion(), serv.getErrorPageByCode(404));
  } else {
    if (access(fileName.c_str(), R_OK) == 0)
      return Response(req.getVersion(), 200, "OK", readPage(fileName));
    else
      return Response(req.getVersion(), serv.getErrorPageByCode(404));
  }
  return Response(req.getVersion(), serv.getErrorPageByCode(404));
}

Response AHttpMethod::DELETE(std::string filename, Request &req, Server &serv) {
  std::string directory = filename.substr(0, filename.find_last_of("/") + 1);
  if (access(directory.c_str(), W_OK) != 0)
    return Response(req.getVersion(), serv.getErrorPageByCode(403));

  if (std::remove(filename.c_str()) != 0)
    return Response(req.getVersion(), serv.getErrorPageByCode(404));

  return Response(req.getVersion(), 200, "OK", "");
}

Response AHttpMethod::POST(std::string filename, Location &loc, Request &req,
                           Server &serv) {
  if (req.getBody().size() * sizeof(char) > loc.getClientMaxBodySize())
    return Response(req.getVersion(), serv.getErrorPageByCode(413));

  std::string directory = filename.substr(0, filename.find_last_of("/") + 1);
  if (access(directory.c_str(), W_OK) != 0)
    return Response(req.getVersion(), serv.getErrorPageByCode(403));

  std::ofstream file(filename.c_str(), std::ostream::out);
  if (!file.is_open())
    return Response(req.getVersion(), serv.getErrorPageByCode(404));
  file << req.getBody() << "OEOE";

  return Response(req.getVersion(), 200, "OK", "");
}