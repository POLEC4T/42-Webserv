/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 12:40:35 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/03 15:04:57 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(): APage() {}

Location::Location(const Location& cpy): APage(cpy) {
	_autoIndex = cpy._autoIndex;
	_index = cpy._index;
	_cgiExtension = cpy._cgiExtension;
	_cgiPath = cpy._cgiPath;
	_allowedMethods.clear();
	for (size_t i = 0; i < cpy._allowedMethods.size(); ++i) {
		if (cpy._allowedMethods[i])
			_allowedMethods.push_back(cpy._allowedMethods[i]);
	}
}

Location&	Location::operator=(const Location& other) {
	if (this != &other)
	{
		this->_name = other._name;
		this->_root = other._root;
		this->_content = other._content;
		this->_code = other._code;
		for (size_t i = 0; i < this->_allowedMethods.size(); ++i) {
			delete this->_allowedMethods[i];
		}
		this->_allowedMethods.clear();
		for (size_t i = 0; i < other._allowedMethods.size(); ++i) {
			if (other._allowedMethods[i])
				this->_allowedMethods.push_back(other._allowedMethods[i]);
		}
		this->_autoIndex = other._autoIndex;
		this->_index = other._index;
	}
	return *this;
}

Location::~Location() {}



//constructor with assignement values
Location::Location(std::string name, std::string root): APage(name, root) {}

Location::Location(std::string name, std::string root,
				std::string content, int code): APage(name, root, content, code) {}



//Setter
void    Location::setAllowedMethods(const std::vector<AHttpMethod*>& methods) {
	for (size_t i = 0; i < _allowedMethods.size(); ++i) {
		delete _allowedMethods[i];
	}
	_allowedMethods.clear();
	for (size_t i = 0; i < methods.size(); ++i) {
		if (methods[i])
			_allowedMethods.push_back(methods[i]);
	}
}

void	Location::setAutoIndex(const bool b) {
	_autoIndex = b;
}

void	Location::setCgiExtension(const std::string& cgiExtension) {
	_cgiExtension = cgiExtension;
}

void	Location::setCgiPath(const std::string& cgiPath) {
	_cgiPath = cgiPath;
}



//Getter
std::vector<AHttpMethod*>   Location::getAllowedMethods() {
    return _allowedMethods;
}

bool	Location::getAutoIndex() {
	return _autoIndex;
}

std::vector<std::string>	Location::getIndex() {
	return _index;
}

std::string	Location::getCgiExtension() {
	return _cgiExtension;
}

std::string	Location::getCgiPath() {
	return _cgiPath;
}



//Vector functions
void    Location::pushMethod(AHttpMethod* method) {
	if (method)
		_allowedMethods.push_back(method);
}

void	Location::addIndex(const std::string& index) {
	_index.push_back(index);
}
