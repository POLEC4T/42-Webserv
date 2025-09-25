/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 12:40:35 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/25 12:59:18 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(): APage() {}

Location::Location(const Location& cpy): APage(cpy) {
	_allowedMethods = cpy._allowedMethods;
	_autoIndex = cpy._autoIndex;
	_index = cpy._index;
	_cgiExtension = cpy._cgiExtension;
	_cgiPath = cpy._cgiPath;
}

Location&	Location::operator=(const Location& other) {
	if (this != &other)
	{
		this->_name = other._name;
		this->_root = other._root;
		this->_content = other._content;
		this->_code = other._code;
		this->_allowedMethods.clear();
		this->_allowedMethods = other._allowedMethods;
		this->_autoIndex = other._autoIndex;
		this->_index = other._index;
		this->_cgiExtension = other._cgiExtension;
		this->_cgiPath = other._cgiPath;
	}
}

Location::~Location() {}

//constructor with assignement values
Location::Location(std::string name, std::string root): APage(name, root) {}

Location::Location(std::string name, std::string root,
				std::string content, int code): APage(name, root, content, code) {}

//Setter
void	Location::setAllowedMethods(const std::vector<AHttpMethod>& methods) {
	_allowedMethods = methods;
}

void	Location::setAutoIndex(const bool autoindex) {
	_autoIndex = autoindex;
}

void	Location::setIndex(const std::string& index) {
	_index = index;
}

void	Location::setCgiExtension(const std::string& cgiExtension) {
	_cgiExtension = cgiExtension;
}

void	Location::setCgiPath(const std::string& cgiPath) {
	_cgiPath = cgiPath;
}

//Getter
std::vector<AHttpMethod>	Location::getAllowedMethods() {
	return _allowedMethods;
}

bool	Location::getAutoIndex() {
	return _autoIndex;
}

std::string	Location::getIndex() {
	return _index;
}

std::string	Location::getCgiExtension() {
	return _cgiExtension;
}

std::string	Location::getCgiPath() {
	return _cgiPath;
}

//Vector functions
void	Location::pushMethod(const AHttpMethod& method) {
	_allowedMethods.push_back(method);
}
