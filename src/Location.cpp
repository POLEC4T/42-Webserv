/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 12:40:35 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/07 16:01:06 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(): APage() {
	_clientMaxBodySize = 0;
	_autoIndex = false;
}

Location::Location(const Location& cpy): APage(cpy) {
	_autoIndex = cpy._autoIndex;
	_index = cpy._index;
	_cgiExtension = cpy._cgiExtension;
	_cgiPath = cpy._cgiPath;
	_return = cpy._return;
	_uploadPath = cpy._uploadPath;
	_clientMaxBodySize = cpy._clientMaxBodySize;
	_allowedMethods = cpy._allowedMethods;
	_index = cpy._index;
}

Location&	Location::operator=(const Location& other) {
	if (this != &other)
	{
		this->_name = other._name;
		this->_root = other._root;
		this->_content = other._content;
		this->_code = other._code;
		this->_return = other._return;
		this->_cgiExtension = other._cgiExtension;
		this->_cgiPath = other._cgiPath;
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



//constructor with assignement values
Location::Location(std::string name, std::string root): APage(name, root) {}

Location::Location(std::string name, std::string root,
				std::string content, int code): APage(name, root, content, code) {}



//Setter

// void    Location::setAllowedMethods(const std::vector<AHttpMethod*>& methods) {
// 	for (size_t i = 0; i < _allowedMethods.size(); ++i) {
// 		delete _allowedMethods[i];
// 	}
// 	_allowedMethods.clear();
// 	for (size_t i = 0; i < methods.size(); ++i) {
// 		if (methods[i])
// 			_allowedMethods.push_back(methods[i]);
// 	}
// }

void	Location::setAutoIndex(const bool b) {
	_autoIndex = b;
}

void	Location::setCgiExtension(const std::string& cgiExtension) {
	_cgiExtension = cgiExtension;
}

void	Location::setCgiPath(const std::string& cgiPath) {
	_cgiPath = cgiPath;
}

void	Location::setClientMaxBodySize(size_t clientMaxBodySize) {
	_clientMaxBodySize = clientMaxBodySize;
}

void	Location::setClientMaxBodySize(std::string clientMaxBodySize) {
	int					maxBodySize = 0;
	std::istringstream	iss(clientMaxBodySize);

	iss >> maxBodySize;
	if (iss.fail())
		throw (Error::IntExpected(clientMaxBodySize));
	setClientMaxBodySize(maxBodySize);
}

void	Location::setReturn(const std::string& ret) {
	_return = ret;
}

void	Location::setUploadPath(const std::string& uploadPath) {
	_uploadPath = uploadPath;
}

//Getter

// std::vector<AHttpMethod*>   Location::getAllowedMethods() {
//     return _allowedMethods;
// }

bool	Location::getAutoIndex() {
	return _autoIndex;
}

const std::vector<std::string>	Location::getIndex() {
	return _index;
}

const std::string&	Location::getCgiExtension() {
	return _cgiExtension;
}

const std::string&	Location::getCgiPath() {
	return _cgiPath;
}

size_t	Location::getClientMaxBodySize() {
	return _clientMaxBodySize;
}

const std::string&	Location::getUploadPath() {
	return _uploadPath;
}

const std::string&	Location::getReturn() {
	return _return;
}

const std::vector<std::string>& Location::getAllowedMethods() const {
    return _allowedMethods;
}

//Vector functions

// void    Location::pushMethod(AHttpMethod* method) {
// 	if (method)
// 		_allowedMethods.push_back(method);
// }

void	Location::addIndex(const std::string& index) {
	_index.push_back(index);
}

void	Location::addAllowedMethods(const std::string& allowedMethod) {
	if (allowedMethod != "GET" && allowedMethod != "POST" && allowedMethod != "DELETE")
		throw (Error::UnknownToken(allowedMethod));
	_allowedMethods.push_back(allowedMethod);
}