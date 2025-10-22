/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:50:02 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/22 15:58:45 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

// #method: check it is in server allowed methods
// mandatory (400)

// #uri
// mandatory (400)

// #version
// on check que c'est HTTP/1.1 ou HTTP/1.0: sinon 400
// mandatory (400)

// #Host:
// HTTP/1.1: 400 Bad Request if missing or bad shape (forbidden character, port
// not in 0-65535 range) HTTP/1.1: mandatory

// GET /index.html HTTP/1.1
// // Method: GET, Path: /index.html, Version: HTTP/1.1 Host: www.example.com
// // Obligatoire en HTTP/1.1 -> sinon 400 Bad Request User-Agent: Mozilla/5.0
// (Windows NT 10.0; Win64; x64) AppleWebKit/537.36	// Accept:
// text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate
// Connection: keep-alive
// Cache-Control: max-age=0
// Content-Length: 348 \r\n
// \r\n
// BODY

// Note: In practice, the "Set-Cookie" header field ([RFC6265]) often
// appears multiple times in a response message and does not use the
// list syntax, violating the above requirements on multiple header
// fields with the same name.  Since it cannot be combined into a
// single field-value, recipients ought to handle "Set-Cookie" as a
// special case while processing header fields.  (See Appendix A.2.3
// of [Kri2001] for details.)
// -> special case for Set-Cookie

# include <map>
# include <iostream>
# include <sstream>
# include <vector>
# include <exception>
# include <cstdlib> 
# include "FtString.hpp" 
# include "RequestExceptions.hpp"

# define MAX_URI_LENGTH 8194

class Server;

class Request {
	private:
		std::string _method;
		std::string _uri;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		bool _parsedRequestLine;
		bool _parsedHeaders;
		bool _parsedBody;
	
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
		bool				parsedRequestLine() const;
		bool				parsedHeaders() const;
		bool				parsedBody() const;
		const std::string&	getBody() const;

};

#endif
