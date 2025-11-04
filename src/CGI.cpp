/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 13:55:23 by mazakov           #+#    #+#             */
/*   Updated: 2025/11/04 11:30:26 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "CGI.hpp"
# include "defines.h"

CGI::CGI(Server& server, Client& client): _server(server), _client(client) {
	int timeOut = server.getTimedOutValue();
	_pid = 0;
	_output = "";
	_fd = -1;
	_timeOut = timeOut != -1 ? timeOut : DEFAULT_TIMEOUT;
	_startTime = time(NULL);
}

CGI::CGI(const CGI& other) : _server(other._server), _client(other._client) {
	_pid = other._pid;
	_fd = other._fd;
	_startTime = other._startTime;
	_timeOut = other._timeOut;
	_output = other._output;
}

CGI::~CGI() {}

Client&	CGI::getClient() {
	return _client;
}

int	CGI::getClientFd() {
	return _client.getFd();
}

Server&	CGI::getServer() {
	return _server;
}

int	CGI::getPid() {
	return _pid;
}

int	CGI::getFd() {
	return _fd;
}

int CGI::getStartTime() {
	return _startTime;
}

int	CGI::getTimeOutValue() {
	return _timeOut;
}

std::string&	CGI::getOutput() {
	return _output;
}

void	CGI::setFd(int fd) {
	_fd = fd;
}

void	CGI::setPid(int pid) {
	_pid = pid;
}

void	CGI::appendOutput(const std::string& s) {
	_output += s;
}