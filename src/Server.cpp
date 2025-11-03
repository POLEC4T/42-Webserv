/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 13:04:32 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/30 15:05:14 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Error.hpp"
#include "defines.h"

Server::Server() {
	_clientMaxBodySize = -1;
	_timedOut = -1;
}

Server::Server(const Server &cpy) {
	_name = cpy._name;
	_timedOut = cpy._timedOut;
	_ports = cpy._ports;
	_host = cpy._host;
	_clientMaxBodySize = cpy._clientMaxBodySize;
	_mapLocation = cpy._mapLocation;
	_mapErrorPage = cpy._mapErrorPage;
	this->_mapDefaultErrorPage = cpy._mapDefaultErrorPage;
	_mapClients = cpy._mapClients;
}

Server &Server::operator=(const Server &other) {
	if (this != &other) {
		this->_timedOut = other._timedOut;
		this->_name = other._name;
		this->_ports = other._ports;
		this->_host = other._host;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_mapLocation = other._mapLocation;
		this->_mapErrorPage = other._mapErrorPage;
		this->_mapDefaultErrorPage = other._mapDefaultErrorPage;
		this->_mapClients = other._mapClients;
	}
	return *this;
}

Server::~Server() {
	deleteAllClients();
	std::vector<int>::iterator it;
	for (it = _sockfds.begin(); it != _sockfds.end(); ++it) {
		if (PRINT)
			std::cout << "close() server fd " << *it << std::endl;
		close(*it);
	}
}

// constructor with assignment values
Server::Server(std::map<int, ErrorPage> errorPages) {
	_clientMaxBodySize = -1;
	_mapDefaultErrorPage = errorPages;
	_timedOut = -1;
}

// Setter
void Server::addName(const std::string &name) { _name.push_back(name); }

void Server::setClientMaxBodySize(int clientMaxBodySize) {
	_clientMaxBodySize = clientMaxBodySize;
}

void Server::setHost(const std::string &host) { _host = host; }

void Server::setClientMaxBodySize(std::string clientMaxBodySize) {
	int maxBodySize = 0;
	std::istringstream iss(clientMaxBodySize);

	iss >> maxBodySize;
	if (!iss.eof())
		throw(Error::IntExpected(clientMaxBodySize));
	setClientMaxBodySize(maxBodySize);
}

void Server::addPort(const std::string &port) {
	int p = 0;
	std::istringstream iss(port);
	iss >> p;
	if (!iss.eof())
		throw(Error::IntExpected(port));
	if (p < 0 || p > 65535)
		throw(Error::IntOutOfRange(port));
	_ports.push_back(port);
}

void Server::setTimeOut(const std::string time) {
	int timedOut = 0;
	std::istringstream iss(time);

	iss >> timedOut;
	if (!iss.eof())
		throw(Error::IntExpected(time));
	if (timedOut < 0 || timedOut > 20)
		throw(Error::IntOutOfRange(time));
	_timedOut = timedOut;
}

// Getter
const std::vector<std::string> &Server::getNames() const { return _name; }

const std::vector<std::string> &Server::getPorts() const { return _ports; }

long long Server::getClientMaxBodySize() const { return _clientMaxBodySize; }

int Server::getTimedOutValue() const { return _timedOut; }

const std::string &Server::getHost() const { return _host; }

// Specific map
void Server::addLocation(const Location &location) {
	_mapLocation.insert(std::make_pair(location.getName(), location));
}

APage &Server::getLocationByName(const std::string &name) {
	std::map<std::string, Location>::iterator it = _mapLocation.find(name);
	if (it == _mapLocation.end()) {
		return (this->getErrorPageByCode(404));
	}
	return it->second;
}

int getContentWithHastag(std::string fileName, std::string &content, char separator) {
	std::string line;
	std::ifstream file(fileName.c_str(), std::ios_base::in);

	if (!file.is_open())
		return 0;
	while (getline(file, line)) {
		if (!line.empty()) {
				content += line;
				content += separator;
			}
		}
	file.close();
	return 0;
}


void Server::addErrorPage(ErrorPage &errorPage) {
	std::string content;
	getContentWithHastag(errorPage.getRoot(), content, ' ');
	errorPage.setContent(content);
	_mapErrorPage[errorPage.getCode()] = errorPage;
}

ErrorPage &Server::getErrorPageByCode(const int code) {
	std::map<int, ErrorPage>::iterator it = _mapErrorPage.find(code);
	if (it == _mapErrorPage.end() || it->second.getContent().empty()) {
		return _mapDefaultErrorPage.find(code)->second;
	}
	return it->second;
}

void Server::addErrorPage(const std::string &name, const std::string &root) {
	ErrorPage errorPage(name, root);
	int code = 0;
	std::istringstream iss(name);

	iss >> code;
	if (!iss.eof())
		throw(Error::IntExpected(name));
	errorPage.setCode(code);
	addErrorPage(errorPage);
}

Client &Server::getClient(int fd) { return (_mapClients[fd]); }

/**
 * @brief adds a new client to the client map
 * The warning should never happen, but just in case
 */
void Server::addClient(const Client &client) {
	if (_mapClients.find(client.getFd()) != _mapClients.end()) {
		if (PRINT) {
			std::cout << "Warning: client with fd " << client.getFd()
					<< " already exists, overwriting it" << std::endl;
		}
		deleteClient(client.getFd());
	}
	_mapClients[client.getFd()] = client;
}

void Server::deleteAllClients() {
	std::map<int, Client>::iterator it;

	for (it = _mapClients.begin(); it != _mapClients.end(); ++it) {
		if (PRINT)
			std::cout << "close() client fd " << it->first << std::endl;
		close(it->first);
	}
	_mapClients.clear();
}

void Server::deleteClient(int fd) {
	if (_mapClients.find(fd) != _mapClients.end()) {
		if (PRINT)
			std::cout << "close() client fd " << fd << std::endl;
		close(fd);
		_mapClients.erase(fd);
	} else {
		if (PRINT)
			std::cout
				<< "Warning: trying to delete non-existing client with fd "
				<< fd << std::endl;
	}
}

void Server::addSockfd(int fd) { _sockfds.push_back(fd); }

const std::vector<int> &Server::getSockfds() const { return _sockfds; }

bool Server::isClient(int fd) const {
	return (_mapClients.find(fd) != _mapClients.end());
}

bool Server::isListener(int fd) const {
	return (std::find(_sockfds.begin(), _sockfds.end(), fd) != _sockfds.end());
}

void Server::parseAndAddLocation(
	std::vector<std::string>::iterator &it,
	const std::vector<std::string>::iterator itEnd) {
	int isClosed = 0;
	Location newLocation;

	newLocation.setName(*it);
	++it;
	while (it != itEnd) {
		if (*it == "{")
			isClosed++;
		else if (*it == "}")
			isClosed--;
		else if (*it == "root") {
			it++;
			if (it != itEnd)
				newLocation.setRoot(*it);
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it == "allowed_methods") {
			it++;
			while (it != itEnd && *it != ";") {
				newLocation.addAllowedMethods(*it);
				it++;
			}
			if (it == itEnd || *it != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it == "index") {
			it++;
			while (it != itEnd && *it != ";") {
				newLocation.addIndex(*it);
				it++;
			}
			if (it == itEnd || *it != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it == "autoindex") {
			it++;
			if (it != itEnd && *it == "on")
				newLocation.setAutoIndex(true);
			else if (it != itEnd && *it == "off")
				newLocation.setAutoIndex(false);
			else
				throw(Error::UnknownToken(*it));
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it == "return") {
			it++;
			if (it != itEnd && (it + 1) != itEnd) {
				newLocation.setReturn(*it + " " + *(it + 1));
				it++;
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw(Error::DidNotFindSemicolon(*(it)));
			it++;
		} else if (*it == "upload_path") {
			it++;
			if (it != itEnd) {
				newLocation.setUploadPath(*it);
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it == "cgi") {
			it++;
			if (it != itEnd && *it != ";" && (it + 1) != itEnd &&
				*(it + 1) != ";") {
				newLocation.addCgi(*it, *(it + 1));
				++it;
			} else
				throw(Error::CgiValuesError());
		} else if (*it == "client_max_body_size") {
			it++;
			if (it != itEnd) {
				newLocation.setClientMaxBodySize(*it);
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it != ";")
			throw(Error::UnknownToken(*it));
		if (isClosed == 0)
			break;
		it++;
	}
	if (isClosed != 0)
		throw(Error::ErrorBracketParseFile());
	addLocation(newLocation);
}

void Server::setDefaultMapErrorPage(const std::map<int, ErrorPage> &map) {
	this->_mapDefaultErrorPage = map;
}

std::map<std::string, Location> &Server::getLocations() { return _mapLocation; }
