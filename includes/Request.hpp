/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:50:02 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/15 11:57:18 by mniemaz          ###   ########.fr       */
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
// HTTP/1.1: 400 Bad Request if missing or bad shape (forbidden character, port not in 0-65535 range)
// HTTP/1.1: mandatory


// GET /index.html HTTP/1.1 												// Method: GET, Path: /index.html, Version: HTTP/1.1
// Host: www.example.com 													// Obligatoire en HTTP/1.1 -> sinon 400 Bad Request
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36	// 
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
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

class Request {
	private:
		std::string _method;
		std::string _uri;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;

		
		std::map<std::string, std::string> _extractHeaders(const std::string &req) const;
		std::string _extractBody(const std::string &req) const;
		void _parseRequestLine(const std::string &reqContent);
	
	public:
		~Request();
		Request();

		void 				parseRequest(const std::string &reqContent);
		void 				displayRequest() const;
		const std::string&	getHeaderValue(const std::string &key) const;
		const std::string& 	getUri() const;

	class NoHeaderValueException : public std::exception {
		private:
			std::string _message;
		public:
			virtual const char* what() const throw();
			NoHeaderValueException(const std::string& key);
			~NoHeaderValueException() throw();
	};

	class BadHeaderNameException : public std::exception {
		private:
			std::string _message;
		public:
			virtual const char* what() const throw();
			BadHeaderNameException(const std::string& key);
			~BadHeaderNameException() throw();
	};

	class RequestLineException : public std::exception {
		virtual const char* what() const throw();
	};

	class NoHeaderColumnException : public std::exception {
		virtual const char* what() const throw();
	};
};

#endif




