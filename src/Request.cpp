/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:34:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/07 18:54:52 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() {}

Request::~Request() {}

Request::Request(const Request &copy) {
	_method = copy._method;
	_location = copy._location;
	_version = copy._version;
	_headers = copy._headers;
	_body = copy._body;
}

Request& Request::operator=(const Request &other) {
	if (this != &other) {
		_method = other._method;
		_location = other._location;
		_version = other._version;
		_headers = other._headers;
		_body = other._body;
	}
	return (*this);
}

std::string Request::_extractLoc(const std::string &req) const {
	std::string loc;
    size_t pos = req.find(' ');
    if (pos == std::string::npos) {
		return loc;
	}
	pos += 1;
	size_t endPos = req.find(" ", pos);
	if (endPos == std::string::npos) {
		return loc;
	}
	loc = req.substr(pos, endPos - pos);
	return loc;
}

std::string Request::_extractMethod(const std::string &req) const {
	std::string method;
	size_t pos = req.find(' ');
	if (pos == std::string::npos) {
		return method;
	}
	method = req.substr(0, pos);
	return method;
}

std::string Request::_extractVersion(const std::string &req) const {
	std::string version;
	size_t pos = req.find(' ');
	if (pos == std::string::npos) {
		return version;
	}
	pos += 1;
	size_t endPos = req.find(" ", pos);
	if (endPos == std::string::npos) {
		return version;
	}
	version = req.substr(pos, endPos - pos);
	return version;
}

std::map< std::string, std::vector<std::string> > Request::_extractHeaders(const std::string &req) const {
	std::map< std::string, std::vector<std::string> > headers;
	size_t startLine = 0;
	size_t endLine = req.find("\r\n");
	std::string line = req.substr(startLine, endLine);

	while (req.find("\r\n", endLine + 2) != endLine + 2) {
		startLine = endLine + 2;
		endLine = req.find("\r\n", startLine);
		line = req.substr(startLine, endLine - startLine);

		size_t endKey;

		endKey = line.find(':');
		if (endKey == std::string::npos)
			throw std::exception();

		std::string key = line.substr(0, endKey);
		std::string value = line.substr(endKey + 1, (line.size() - 1) - endKey);


		std::cout << "headers[" << key << "] = '" << value << "'" << std::endl;


		headers[key].push_back(value);
	}


	return headers;
}

Request::Request(const std::string &reqContent) {

	_method = _extractMethod(reqContent);
	_location = _extractLoc(reqContent);
	_version = _extractVersion(reqContent);
	_headers = _extractHeaders(reqContent);
	if (_headers["Content-Size"].size() > 0 && _headers["Content-Size"][0].size() > 0) {
		// _body = _extractBody(reqContent);
	}
}