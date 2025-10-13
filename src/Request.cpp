/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:34:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/13 17:47:03 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::~Request() {}

Request::Request() {}

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

		size_t columnIdx;

		columnIdx = line.find(':');
		if (columnIdx == std::string::npos)
			throw NoHeaderColumnException();

		FtString key = line.substr(0, columnIdx);
		FtString value = line.substr(columnIdx + 1, line.size() - (columnIdx + 1));

		key.ltrim();
		if (key.empty() || key.endsWith(" "))
			throw BadHeaderNameException(key);

		value.trim();
		if (value.empty())
			throw NoHeaderValueException(key);

		headers[key].push_back(value);
	}
	return headers;
}

const std::string& Request::_getHeaderValue(const std::string &key) const {
	std::map<std::string, std::vector<std::string> >::const_iterator it = _headers.find(key);
	if (it == _headers.end())
		throw NoHeaderValueException(key);
	return (it->second[0]);
}

std::string Request::_extractBody(const std::string &req) const {
	std::string bodySize = _getHeaderValue("Content-Size");

	size_t startBody = req.find("\r\n\r\n") + 4;
	size_t sizeBody = atoi(bodySize.c_str());
	std::string body = req.substr(startBody, sizeBody);
	return body;
}


/**
 * @throws if:
 *		request line is not exactly "<method> <uri> <version>\r\n"
 *		method is not GET, POST or DELETE
 *		version is not be HTTP/1.1 or HTTP/1.0
 */
void Request::_parseRequestLine(const std::string &reqContent) {
	std::istringstream reqContentISS(reqContent);
	FtString reqLine;
	std::getline(reqContentISS, reqLine);

	if (reqLine.find('\r') != reqLine.size() - 1)
		throw RequestLineException();
	reqLine.erase(reqLine.end() - 1);

	if (reqLine.startsOrEndsWith(" "))
		throw RequestLineException();

	size_t firstSpace = reqLine.find(" ");
	size_t lastSpace = reqLine.rfind(" ");

	bool hasNotThreeElems = firstSpace == std::string::npos
							|| lastSpace == std::string::npos
							|| firstSpace == lastSpace;
	if (hasNotThreeElems)
		throw RequestLineException();

	_method = reqLine.substr(0, firstSpace);
	_uri = reqLine.substr(firstSpace + 1, lastSpace - (firstSpace + 1));
	_version = reqLine.substr((lastSpace + 1), (reqLine.size()) - (lastSpace + 1));

	if (_uri.empty() || _uri.find(' ') != std::string::npos)
		throw RequestLineException();
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		throw RequestLineException();
	if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
		throw RequestLineException();

	
}

void Request::init(const std::string &reqContent) {

	_parseRequestLine(reqContent);
	_headers = _extractHeaders(reqContent);
	if (_headers["Content-Size"].size() > 0 && _headers["Content-Size"][0].size() > 0) {
		_body = _extractBody(reqContent);
	}
}

void Request::displayRequest() const {
	std::cout << "Method: '" << _method << "'" << std::endl;
	std::cout << "URI: '" << _uri << "'" << std::endl;
	std::cout << "Version: '" << _version << "'" << std::endl;
	std::cout << "Headers: " << std::endl;
	for (std::map< std::string, std::vector<std::string> >::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
		std::cout << "  '" << it->first << "': '";
		for (size_t i = 0; i < it->second.size(); ++i) {
			std::cout << it->second[i];
			if (i < it->second.size() - 1)
				std::cout << ", ";
		}
		std::cout << "'" << std::endl;
	}
	std::cout << "Body: '" << _body << "'" << std::endl;
}

const std::string& Request::getUri() const {
	return _uri;
}

const char* Request::NoHeaderValueException::what() const throw() {
	if (_message.empty())
		return "Request: No header field for this key";
	return _message.c_str();
}

Request::NoHeaderValueException::NoHeaderValueException(const std::string& missingKey) {
	_message = "Request: No header field for \"" + missingKey + "\"";
}

Request::NoHeaderValueException::~NoHeaderValueException() throw() {}

const char* Request::RequestLineException::what() const throw() {
	return "Request: Error in the request line.";
}

const char* Request::NoHeaderColumnException::what() const throw() {
	return "Request: A header column is missing.";
}

const char* Request::BadHeaderNameException::what() const throw() {
	if (_message.empty())
		return "Request: bad header name";
	return _message.c_str();
}

Request::BadHeaderNameException::BadHeaderNameException(const std::string& headerName) {
	if (headerName.empty())
		_message = "Request: bad header name: empty";
	else
	_message = "Request: bad header name: \"" + headerName + "\"";
}

Request::BadHeaderNameException::~BadHeaderNameException() throw() {}