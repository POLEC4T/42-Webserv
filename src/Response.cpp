/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dorianmazari <dorianmazari@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 17:47:45 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/20 13:41:11 by dorianmazar      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "ErrorPage.hpp"
#include "FtString.hpp"

Response::Response() {}

Response::Response(std::string v, int c, std::string s, std::string b) {
  this->_status = s;
  this->_code = c;
  this->_version = v;
  this->_body = b;
  this->_headers["Content-Length"] = FtString::my_to_string(b.size());
  this->_headers["Content-type"] = "text/html";
}

Response::Response(std::string v, ErrorPage &page) {
  this->_status = page.getName();
  this->_code = page.getCode();
  this->_version = v;
  this->_body = page.getContent();
  this->_headers["Content-Length"] =
      FtString::my_to_string(page.getContent().size());
  this->_headers["Content-type"] = "text/html";
}

Response &Response::operator=(const Response &other) {
  if (this != &other) {
    this->_status = other._status;
    this->_code = other._code;
    this->_version = other._version;
    this->_body = other._body;
    this->_headers = other._headers;
  }
  return *this;
}

void Response::setHeader(const std::string &key, const std::string &value) {
  this->_headers[key] = value;
}

void Response::setBody(const std::string &body) { this->_body = body; }

std::string Response::build() {
  std::string res;
  std::stringstream stream;
  stream << this->_code;
  std::string str;
  stream >> str;

  res = this->_version + ' ' + str + ' ' + this->_status + "\r\n";
  for (std::map<std::string, std::string>::iterator it = this->_headers.begin();
       it != this->_headers.end(); it++) {
    res += it->first;
    res += ": ";
    res += it->second;
    res += "\r\n";
  }
  res += "\r\n";
  res += _body;

  return res;
}

std::string Response::getVersion() const { return (this->_version); }

int Response::getCode() const { return this->_code; }

std::string Response::getStatus() const { return this->_status; }

std::string Response::getBody() const { return this->_body; }

std::map<std::string, std::string> Response::getHeaders() const {
  return this->_headers;
}

Response::~Response() {}
