/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 17:47:45 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/14 12:58:02 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(){}

Response::Response(std::string v, int c, std::string s)
{
    this->_status = s;
    this->_code = c;
    this->_version = v;
}

Response& operator=(const Response& other)
{
    if (this != &other)
    {
        this->_status = other._status;
        this->_code = other._code;
        this->_version = other._version;
        this->_body = other._body;
        this->_headers = other._headers;
        this->_statusLine = other._statusLine;
    }
    return *this;
}

void    Response::setHeader(const std::string& key, const std::string& value)
{
    this->headers[key] = value;
}

void    Response::setBody(const std::string& body)
{
    this->_body = body;
}

std::string Response::build()
{
    std::string res;
    
    res = this->_version + ' ' + to_string(this->_code) + ' ' + this->_status + '\r\n';
    for (std::map<std::string, std::string>::iterator it = this->map.begin(); it != this->map.end(); it++)
    {
        res += *it->first;
        res += ": ";
        res += *it->second;
        res += '\r\n';
    }
    res += '\r\n';
    res += body;
}

Response::~Response(){}

