/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:34:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/20 18:39:35 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Server.hpp"

Request::Request() :
_parsedRequestLine(false),
_parsedHeaders(false),
_parsedBody(false) {}

Request::~Request() {}

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
void Request::parseHeaders(const std::string &req) {
	size_t startLine = 0;
	size_t endLine = req.find("\r\n");
	std::string line = req.substr(startLine, endLine);

	while (req.find("\r\n", endLine + 2) != endLine + 2) {
		startLine = endLine + 2;
		endLine = req.find("\r\n", startLine);
		line = req.substr(startLine, endLine - startLine);

		size_t columnIdx = line.find(':');
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

		_headers[key] = value;
	}
	_parsedHeaders = true;
}

/**
 * @throws if key not found
 */
const std::string& Request::getHeaderValue(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it == _headers.end())
		throw NoHeaderValueException(key);
	return (it->second);
}

void Request::parseBody(const std::string &req, int bodyLength) {
	size_t startBody = req.find("\r\n\r\n") + 4;
	_body = req.substr(startBody, bodyLength);
	_parsedBody = true;
}


/**
 * @throws if:
 *		request line is not exactly "<method> <uri> <version>\r\n"
 *		method is not GET, POST or DELETE
 *		version is not HTTP/1.1 or HTTP/1.0
 */
void Request::parseRequestLine(const std::string &reqContent) {
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
	_parsedRequestLine = true;
}

void Request::displayRequest() const {
	std::cout << "Method: '" << _method << "'" << std::endl;
	std::cout << "URI: '" << _uri << "'" << std::endl;
	std::cout << "Version: '" << _version << "'" << std::endl;
	std::cout << "Headers: " << std::endl;
	for (std::map< std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
		std::cout << "  '" << it->first << "': '" << it->second << "'" << std::endl;
	}
	std::cout << "Body: '" << _body << "'" << std::endl;
}

const std::string& Request::getUri() const {
	return _uri;
}

const std::string&  Request::getMethod() const
{
	return _method;
}

const std::string&  Request::getVersion() const
{
	return _version;
}

bool				Request::parsedRequestLine() const {
	return _parsedRequestLine;
}

bool				Request::parsedHeaders() const {
	return _parsedHeaders;	
}

bool				Request::parsedBody() const {
	return _parsedBody;	
}


const std::string&  Request::getBody() const
{
	return _body;
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


const char* Request::MaxBodySizeExceededException::what() const throw() {
	return "Body size exceeded the maximum allowed size.";
}
