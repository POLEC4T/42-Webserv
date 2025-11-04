/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:50:02 by mniemaz           #+#    #+#             */
/*   Updated: 2025/11/04 14:35:47 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "FtString.hpp"
#include "ParsePacketExceptions.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#define MAX_URI_LENGTH 8194

class Server;

class Request {
private:
	std::string 						_method;
	std::string 						_uri;
	std::string 						_version;
	std::map<std::string, std::string>	_headers;
	std::string 						_body;
	bool 								_parsedRequestLine;
	bool 								_parsedHeaders;
	bool 								_parsedBody;
	time_t								_startTime; // is set to the time we received the first packet of the request

public:
	Request();
	~Request();

	void				parseHeaders(const std::string &reqContent);
	void				parseBody(const std::string &reqContent, size_t bodyLength);
	void				parseRequestLine(const std::string &reqContent);
	void 				displayRequest() const;
	std::string			getHeaderValue(const std::string &key) const;
	const std::string& 	getUri() const;
	const std::string& 	getMethod() const;
	const std::string&	getVersion() const;
	const std::string&	getBody() const;
	bool				parsedRequestLine() const;
	bool				parsedHeaders() const;
	bool				parsedBody() const;
	void				appendBody(const std::string& toadd);
	void				setStartTime(time_t);
	bool				hasTimedOut(time_t maxTime) const;
};

#endif
