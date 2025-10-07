/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 13:04:32 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/07 11:51:08 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {}

Server::Server(const Server& cpy) {
	_name = cpy._name;
	_port = cpy._port;
	_clientMaxBodySize = cpy._clientMaxBodySize;
	_mapLocation = cpy._mapLocation;
	_mapErrorPage = cpy._mapErrorPage;
}

Server&	Server::operator=(const Server& other) {
	if (this != &other)
	{
		this->_name = other._name;
		this->_port = other._port;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_mapLocation = other._mapLocation;
		this->_mapErrorPage = other._mapErrorPage;
	}
	return *this;
}

Server::~Server() {}



//constructor with assignment values
Server::Server(int port, int clientMaxBodySize) {
	_port = port;
	_clientMaxBodySize = clientMaxBodySize;
}



//Setter
void	Server::addName(std::string name) {
	_name.push_back(name);
}

void	Server::setPort(int port) {
	_port = port;
}

void	Server::setClientMaxBodySize(int clientMaxBodySize) {
	_clientMaxBodySize = clientMaxBodySize;
}

void	Server::setHost(std::string host) {
	_host = host;
}

void	Server::setClientMaxBodySize(std::string clientMaxBodySize) {
	int					maxBodySize = 0;
	std::istringstream	iss(clientMaxBodySize);

	iss >> maxBodySize;
	setClientMaxBodySize(maxBodySize);
}

void	Server::setPort(std::string port) {
	int					p = 0;
	std::istringstream	iss(port);

	iss >> p;
	setPort(p);
}

//Getter
std::vector<std::string>	Server::getNames() {
	return _name;
}

int	Server::getPort() {
	return _port;
}

int	Server::getClientMaxBodySize() {
	return _clientMaxBodySize;
}

std::string	Server::getHost() {
	return _host;
}

//Specific map
void	Server::addLocation(const Location& location) {
	_mapLocation.insert(std::make_pair(location.getName(), location));
}

APage&	Server::getLocationByName(const std::string& name) {
	std::map<std::string, Location>::iterator it = _mapLocation.find(name);
	if (it == _mapLocation.end())
	{
		return (this->getErrorPageByCode(404));
	}
	return it->second;
}

void	Server::addErrorPage(const ErrorPage& errorPage) {
	_mapErrorPage.insert(std::make_pair(errorPage.getCode(), errorPage));
}

APage&	Server::getErrorPageByCode(const int code) {
	std::map<int, ErrorPage>::iterator it = _mapErrorPage.find(code);
	if (it == _mapErrorPage.end())
	{
		throw(NoPageFound());
	}
	return it->second;
}

void	Server::addErrorPage(std::string& name, std::string& root) {
	ErrorPage			errorPage(name, root);
	int					code = 0;
	std::istringstream	iss(name);

	iss >> code;

	errorPage.setCode(code);
	addErrorPage(errorPage);
}

void	Server::configFileLocationParser(std::vector<std::string>::iterator& it, const std::vector<std::string>::iterator itEnd) {
	int	isClosed = 0;
	Location	newLocation;

	newLocation.setName(*it);
	++it;
	while (it != itEnd)
	{
		if (*it == "{")
			isClosed++;
		else if (*it == "}")
			isClosed--;
		if (isClosed == 0)
			break;
		if (*it == "root") {
			it++;
			if (it != itEnd)
				newLocation.setRoot(*it);
		}
		else if (*it == "allowed_methods") {
			it++;
			while (it != itEnd && *it != ";") {
				newLocation.addAllowedMethods(*it);
				it++;
			}
		}
		else if (*it == "index") {
			it++;
			while (it != itEnd && *it != ";") {
				newLocation.addIndex(*it);
				it++;
			}
		}
		else if (*it == "autoindex") {
			it++;
			if (it != itEnd && *it == "on")
				newLocation.setAutoIndex(true);
			else if (it != itEnd && *it == "off")
				newLocation.setAutoIndex(false);
		}
		else if (*it == "return") {
			it++;
			if (it != itEnd && (it + 1) != itEnd) {
				newLocation.setReturn(*it);
			}
		}
		else if (*it == "upload_path") {
			it++;
			if (it != itEnd) {
				newLocation.setUploadPath(*it);
			}
		}
		else if (*it == "cgi_extension") {
			it++;
			if (it != itEnd) {
				newLocation.setCgiExtension(*it);
			}
		}
		else if (*it == "cgi_path") {
			it++;
			if (it != itEnd) {
				newLocation.setCgiPath(*it);
			}
		}
		it++;
	}
	addLocation(newLocation);
}

std::map<std::string, Location>& Server::getLocations() {
    return _mapLocation;
}

//exception class
const char*	Server::NoPageFound::what() const throw() {
	return "Page and error page not found";
}


//functions
