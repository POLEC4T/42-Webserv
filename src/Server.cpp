/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 13:04:32 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/07 11:56:12 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {}

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

const std::string&	Server::getHost() const {
	return _host;
}

//Specific map
void	Server::pushLocation(const Location& location) {
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

void	Server::pushErrorPage(const ErrorPage& errorPage) {
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
	pushErrorPage(errorPage);
}

//exception class
const char*	Server::NoPageFound::what() const throw() {
	return "Page and error page not found";
}


//functions
