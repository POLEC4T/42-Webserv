/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:34:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/09 13:28:07 by mniemaz          ###   ########.fr       */
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
	pos = req.find(' ', pos);
	if (pos == std::string::npos) {
		return version;
	}
	pos += 1;
	size_t endPos = req.find("\r\n", pos);
	if (endPos == std::string::npos) {
		return version;
	}
	version = req.substr(pos, endPos - pos);
	return version;
}

/** RFC 7230:
 * - check if header must be unique (Content-Length, Host, etc) 400
 * - No whitespace is allowed between the header field-name and colon : 400
 * - whitespace before and after value : ca degage au parsing
 * -  A server that receives an obs-fold in a request message that is not
   within a message/http container MUST either reject the message by
   sending a 400 (Bad Request) (obs-fold ou obselete-folding c'est quand
   la value est \n, et que la ligne d'apres commence par un espace ou un \t)
 * - est ce qu'on a une taille maximale de ligne ? (key + value)
 * - value: (check RFC 7230: 3.2.6)
 * 		check valid chars 
 * 		handle quoted strings "..." 
 * 		handle comments (...)
 * 		put \ in front of wanted chars
 * 		handle spaces and ','
 * 		sometimes / is valid and other times not ????? type MIME allows it -> WHAATTTTT : User-AgentMozilla/5.0 Chaîne libre (souvent avec /, mais pas un type MIME).
 * 		' ' are valid, but only to separate tokens, quoted-strings and comments 
 * - discuter du header Transfer-Encoding, ca a l'air hyper complexe, est ce qu'on doit vraiment le faire ? ou on va s'arreter ?
 */
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
		headers[key].push_back(value);
	}
	return headers;
}

const std::string& Request::_getHeaderValue(const std::string &key) const {
	std::map<std::string, std::vector<std::string> >::const_iterator it = _headers.find(key);
	if (it == _headers.end())
		throw NoHeaderValue();
	return (it->second[0]);
}

std::string Request::_extractBody(const std::string &req) const {
	std::string bodySize = _getHeaderValue("Content-Size");

	size_t startBody = req.find("\r\n\r\n") + 4;
	size_t sizeBody = atoi(bodySize.c_str());
	std::string body = req.substr(startBody, sizeBody);
	return body;
}

void Request::_parseRequestLine(const std::string &reqContent) {
	std::istringstream iss(reqContent);
	
	std::string requestLine;
	std::getline(iss, requestLine);
	std::cout << "req line: " << requestLine << std::endl;

	if (requestLine.empty() || requestLine.find('\r') == std::string::npos) {
	
	}
}

Request::Request(const std::string &reqContent) {

	_parseRequestLine(reqContent);
	_method = _extractMethod(reqContent);
	_location = _extractLoc(reqContent);
	_version = _extractVersion(reqContent);
	_headers = _extractHeaders(reqContent);
	if (_headers["Content-Size"].size() > 0 && _headers["Content-Size"][0].size() > 0) {
		_body = _extractBody(reqContent);
	}
}

void Request::displayRequest() const {
	std::cout << "Method: '" << _method << "'" << std::endl;
	std::cout << "Location: '" << _location << "'" << std::endl;
	std::cout << "Version: '" << _version << "'" << std::endl;
	std::cout << "Headers: " << std::endl;
	for (std::map< std::string, std::vector<std::string> >::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
		std::cout << "  " << it->first << ": ";
		for (size_t i = 0; i < it->second.size(); ++i) {
			std::cout << it->second[i];
			if (i < it->second.size() - 1)
				std::cout << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << "Body: '" << _body << "'" << std::endl;

}

const char* Request::NoHeaderValue::what() const throw() {
	return "Request: getHeaderValue: No header has this key";
}