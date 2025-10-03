/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 13:04:32 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/03 23:17:32 by mazakov          ###   ########.fr       */
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

//Getter
std::vector<std::string>	Server::getName() {
	return _name;
}

int	Server::getPort() {
	return _port;
}

int	Server::getClientMaxBodySize() {
	return _clientMaxBodySize;
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



// //functions
// void	Server::fillServerContent(const char* filename) {
// 	std::ifstream	file(filename, std::ios::in);
// 	std::string		content;
// 	if (!file.is_open())
// 		throw (CanNotOpenFile());
	
// 	file.close();
// }



//exception class
const char*	Server::NoPageFound::what() const throw() {
	return "Page and error page not found";
}
