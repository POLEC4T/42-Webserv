/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 12:40:35 by mazakov           #+#    #+#             */
/*   Updated: 2025/11/04 15:59:57 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "Error.hpp"

Location::Location() : APage(200) {
	_clientMaxBodySize = -1;
	_autoIndex = false;
}

Location::Location(const Location &cpy) : APage(cpy) {
	_autoIndex = cpy._autoIndex;
	_index = cpy._index;
	_cgi = cpy._cgi;
	_return = cpy._return;
	_uploadPath = cpy._uploadPath;
	_clientMaxBodySize = cpy._clientMaxBodySize;
	_allowedMethods = cpy._allowedMethods;
	_index = cpy._index;
}

Location &Location::operator=(const Location &other) {
	if (this != &other) {
		this->_name = other._name;
		this->_root = other._root;
		this->_content = other._content;
		this->_code = other._code;
		this->_return = other._return;
		this->_cgi = other._cgi;
		this->_allowedMethods.clear();
		this->_allowedMethods = other._allowedMethods;
		this->_index.clear();
		this->_index = other._index;
		this->_autoIndex = other._autoIndex;
		this->_index = other._index;
		this->_clientMaxBodySize = other._clientMaxBodySize;
	}
	return *this;
}

Location::~Location() {}

// constructor with assignement values
Location::Location(std::string name, std::string root) : APage(name, root) {
	this->_clientMaxBodySize = -1;
}

Location::Location(std::string name, std::string root, std::string content,
				int code)
	: APage(name, root, content, code) {
	this->_clientMaxBodySize = -1;
}

// Setter

void Location::setAutoIndex(const bool b) { _autoIndex = b; }

void Location::addCgi(const std::string &extension, const std::string &path) {
	_cgi[extension] = path;
}

void Location::setClientMaxBodySize(size_t clientMaxBodySize) {
	_clientMaxBodySize = clientMaxBodySize;
}

void Location::setClientMaxBodySize(std::string clientMaxBodySize) {
	int maxBodySize = 0;
	std::istringstream iss(clientMaxBodySize);

	iss >> maxBodySize;
	if (!iss.eof())
		throw(Error::IntExpected(clientMaxBodySize));
	setClientMaxBodySize(maxBodySize);
}

void Location::setReturn(const std::string &ret) { _return = ret; }

void Location::setUploadPath(const std::string &uploadPath) {
	_uploadPath = uploadPath;
}

// Getter
bool Location::getAutoIndex() { return _autoIndex; }

const std::vector<std::string> Location::getIndex() { return _index; }

const std::string &Location::getCgiByExtension(const std::string& extension) const {
	static const std::string empty;
	std::map<std::string, std::string>::const_iterator it = _cgi.find(extension);
	if (it == _cgi.end())
		return empty;
	return it->second;
}

long long Location::getClientMaxBodySize() const { return _clientMaxBodySize; }

const std::string &Location::getUploadPath() { return _uploadPath; }

const std::string &Location::getReturn() { return _return; }

const std::vector<std::string> &Location::getAllowedMethods() const {
	return _allowedMethods;
}

void Location::addIndex(const std::string &index) { _index.push_back(index); }

void Location::addAllowedMethods(const std::string &allowedMethod) {
	if (allowedMethod != "GET" && allowedMethod != "POST" &&
		allowedMethod != "DELETE")
		throw(Error::UnknownToken(allowedMethod));
	_allowedMethods.push_back(allowedMethod);
}