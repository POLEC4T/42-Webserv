/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 13:04:32 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/13 18:32:08 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {
	_clientMaxBodySize = 0;
	_port = 0;
}

Server::Server(const Server& cpy) {
	_name = cpy._name;
	_port = cpy._port;
	_host = cpy._host;
	_clientMaxBodySize = cpy._clientMaxBodySize;
	_mapLocation = cpy._mapLocation;
	_mapErrorPage = cpy._mapErrorPage;
}

Server&	Server::operator=(const Server& other) {
	if (this != &other)
	{
		this->_name = other._name;
		this->_port = other._port;
		this->_host = other._host;
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

Server::Server(std::map<int, ErrorPage> errorPages) {
	_mapDefaultErrorPage = errorPages;
}


//Setter
void	Server::addName(const std::string& name) {
	_name.push_back(name);
}

void	Server::setPort(int port) {
	_port = port;
}

void	Server::setClientMaxBodySize(int clientMaxBodySize) {
	_clientMaxBodySize = clientMaxBodySize;
}

void	Server::setHost(const std::string& host) {
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
	if (iss.fail())
		throw (Error::IntExpected(port));
	setPort(p);
}

//Getter
const std::vector<std::string>&	Server::getNames() const {
	return _name;
}

int	Server::getPort() const {
	return _port;
}

int	Server::getClientMaxBodySize() const {
	return _clientMaxBodySize;
}

const std::string&	Server::getHost() const {
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
		return _mapDefaultErrorPage[code];
	}
	return it->second;
}

void	Server::addErrorPage(const std::string& name, const std::string& root) {
	ErrorPage			errorPage(name, root);
	int					code = 0;
	std::istringstream	iss(name);

	iss >> code;
	if (iss.fail())
		throw (Error::IntExpected(name));
	errorPage.setCode(code);
	addErrorPage(errorPage);
}

void	Server::parseAndAddLocation(std::vector<std::string>::iterator& it, const std::vector<std::string>::iterator itEnd) {
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
		else if (*it == "root") {
			it++;
			if (it != itEnd)
				newLocation.setRoot(*it);
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "allowed_methods") {
			it++;
			while (it != itEnd && *it != ";") {
				newLocation.addAllowedMethods(*it);
				it++;
			}
			if (it == itEnd || *it != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "index") {
			it++;
			while (it != itEnd && *it != ";") {
				newLocation.addIndex(*it);
				it++;
			}
			if (it == itEnd || *it != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "autoindex") {
			it++;
			if (it != itEnd && *it == "on")
				newLocation.setAutoIndex(true);
			else if (it != itEnd && *it == "off")
				newLocation.setAutoIndex(false);
			else
				throw (Error::UnknownToken(*it));
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "return") {
			it++;
			if (it != itEnd && (it + 1) != itEnd) {
				newLocation.setReturn(*it + " " + *(it + 1));
				it++;
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "upload_path") {
			it++;
			if (it != itEnd) {
				newLocation.setUploadPath(*it);
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "cgi_extension") {
			it++;
			if (it != itEnd) {
				newLocation.setCgiExtension(*it);
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "cgi_path") {
			it++;
			if (it != itEnd) {
				newLocation.setCgiPath(*it);
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "client_max_body_size") {
			it++;
			if (it != itEnd) {
				newLocation.setClientMaxBodySize(*it);
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it != ";")
			throw (Error::UnknownToken(*it));
		if (isClosed == 0)
			break;
		it++;
	}
	if (isClosed != 0)
		throw (Error::ErrorBracketParseFile());
	addLocation(newLocation);
}

std::map<std::string, Location>& Server::getLocations() {
    return _mapLocation;
}
