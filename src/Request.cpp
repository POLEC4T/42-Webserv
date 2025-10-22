/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:34:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/22 17:21:34 by mniemaz          ###   ########.fr       */
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
/**
 * @throws if no ':' found in a header line
 * @throws if header name is invalid (empty or ends with space)
 * @throws if header value is empty
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
 * @returns empty string if header not found
 */
std::string Request::getHeaderValue(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return (it->second);
	return "";
}

void Request::parseBody(const std::string &req, size_t bodyLength) {
	size_t startBody = req.find("\r\n\r\n") + 4;
	_body = req.substr(startBody, bodyLength);
	_parsedBody = true;
}


/**
 * @throws if request line does not end with \r\n
 * @throws if request line starts or ends with space
 * @throws if request line is not exactly "<method> <uri> <version>\r\n"
 * @throws if method is not GET, POST or DELETE
 * @throws if version is not HTTP/1.1 or HTTP/1.0
 */
void Request::parseRequestLine(const std::string &reqContent) {
	std::istringstream reqContentISS(reqContent);
	FtString reqLine;
	std::getline(reqContentISS, reqLine);

	if (reqLine.find('\r') != reqLine.size() - 1)
		throw RequestLineException();
	reqLine.erase(reqLine.end() - 1);

	if (reqLine.startsOrEndsWith(WHITESPACES))
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

	if (_uri.size() > MAX_URI_LENGTH) 
		throw UriTooLongException();
	if (_uri.empty() || _uri.find(SP) != std::string::npos)
		throw RequestLineException();
	if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
		throw HttpVersionNotSupportedException();
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