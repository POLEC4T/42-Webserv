/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodExecutor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:30:23 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/30 15:05:39 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MethodExecutor.hpp"
#include "AHttpMethod.hpp"

bool isCGI(Request &req, Location &loc);
std::string CGIHandler(Request &req, Location &loc, Server &serv,
					Client &client);
std::string readPage(std::string fileName);

MethodExecutor::MethodExecutor(Server &s, Client &c) : _server(s), _client(c) {
	this->_request = this->_client.getRequest();
	this->_method = this->_request.getMethod();
}

Location MethodExecutor::getRequestLocation(Request &req, Server &serv) {
	std::string path(req.getUri());
	std::map<std::string, Location> &locations = serv.getLocations();

	std::map<std::string, Location>::iterator it = locations.find(path);
	if (it != locations.end())
		return it->second;
	size_t l = path.find_last_of('/');
	if (l == 0)
		path = "/";
	else if (l != std::string::npos)
		path = path.substr(0, l);
	while (1) {
		for (std::map<std::string, Location>::iterator it = locations.begin();
			it != locations.end(); it++) {
			if (it->first == path)
				return it->second;
		}
		if (path.empty() || path == "/")
			break;

		l = path.find_last_of('/');
		if (l == 0)
			path = "/";
		else if (l != std::string::npos)
			path = path.substr(0, l);
		else
			break;
	}
	Location loc;
	loc.setCode(404);
	return loc;
}

Response& MethodExecutor::getResponse() { return this->_response; }

int returnHandler(Response &response, Location &loc, Request &req,
				Server &serv) {
	if (loc.getReturn().size() == 0)
		return 1;

	std::vector<std::string> split =
		static_cast<FtString>(loc.getReturn()).ft_split(" ");
	int i = 0;
	std::istringstream stream(split[0]);
	stream >> i;
	if (!stream.eof() || (i < 300 || i > 308)) {
		response =
			Response(req.getVersion(), serv.getErrorPageByCode(BAD_REQUEST));
		return 0;
	}

	response = Response(req.getVersion(), std::atoi(split[0].c_str()),
						"Redirection", "");
	response.setHeader("Location", split[1]);
	return 0;
}

std::string MethodExecutor::execute() {
	std::cout << "URI " << _request.getUri() << std::endl;
	Location loc = this->getRequestLocation(this->_request, this->_server);
	if (loc.getCode() == PAGE_NOT_FOUND)
		return Response(this->_request.getVersion(), this->_server.getErrorPageByCode(PAGE_NOT_FOUND))
			.build();
	if (returnHandler(this->_response, loc, this->_request, this->_server) == 0)
		return this->_response.build();
	std::string fileName(loc.getRoot());
	fileName += this->_request.getUri();

	if (isCGI(this->_request, loc))
		return CGIHandler(this->_request, loc, this->_server, this->_client);
	if (this->_method == "GET" && std::find(loc.getAllowedMethods().begin(), loc.getAllowedMethods().end(), "GET") != loc.getAllowedMethods().end())
		this->_response = AHttpMethod::GET(fileName, loc, this->_request, this->_server);
	else if (this->_method == "POST" && std::find(loc.getAllowedMethods().begin(), loc.getAllowedMethods().end(), "POST") != loc.getAllowedMethods().end())
		this->_response = AHttpMethod::POST(fileName, this->_request, this->_server);
	else if (this->_method == "DELETE" && std::find(loc.getAllowedMethods().begin(), loc.getAllowedMethods().end(), "DELETE") != loc.getAllowedMethods().end())
		this->_response = AHttpMethod::DELETE(fileName, this->_request, this->_server);
	else
		this->_response = Response(this->_request.getVersion(), this->_server.getErrorPageByCode(METHOD_NOT_ALLOWED));
	return this->_response.build();
}

MethodExecutor::~MethodExecutor() {}