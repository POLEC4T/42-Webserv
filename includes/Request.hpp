/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:50:02 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/07 17:43:44 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

// #method: check it is in server allowed methods
// mandatory (400)

// #location 
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

# include <map>
# include <iostream>
# include <vector>

class Request {
	private:
		std::string _method;
		std::string _location;
		std::string _version;
		std::map< std::string, std::vector<std::string> > _headers;
		std::string _body;

		std::string _extractLoc(const std::string &req) const;
		std::string _extractMethod(const std::string &req) const;
		std::string _extractVersion(const std::string &req) const;
		std::map< std::string, std::vector<std::string> > _extractHeaders(const std::string &req) const;
		std::string _extractBody(const std::string &req) const;
		
	
	public:
		Request();
		~Request();
		Request(const Request &copy);
		Request& operator=(const Request &other);

		Request(const std::string &reqContent);
};

#endif




