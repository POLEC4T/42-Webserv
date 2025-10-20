/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 10:08:09 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/20 17:22:25 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _status(WAITING), _fd(-1) {};

Client::Client(int fd) : _status(WAITING), _fd(fd) {};

Client::~Client() {};

int 	Client::getFd() const {
	return (_fd);
}

const std::string&	Client::getBuffer() const {
	return (_buffer);
}

t_client_status		Client::getStatus() const {
	return (_status);
}

void Client::setStatus(t_client_status status) {
	_status = status;
}


void Client::appendBuffer(char *buffer) {
	_buffer.append(buffer);
}

void Client::appendBuffer(const char *buffer) {
	_buffer.append(buffer);
}

void Client::clearBuffer() {
	_buffer.clear();
}

/**
 * @brief gets the value after "Content-Length:"
 */
int Client::_getContentLength() const {
	size_t contentLengthPos =  _buffer.find("Content-Length");
	if (contentLengthPos == std::string::npos)
		return (-1);
	size_t colPos = _buffer.find(":", contentLengthPos);
	if (colPos == std::string::npos)
		return (-1);
	size_t endlPos = _buffer.find("\r\n", colPos);
	if (endlPos == std::string::npos)
		return (-1);

	if (colPos > endlPos)
		return (-1);

	std::string clStr = _buffer.substr((colPos + 1), endlPos - (colPos + 1));

	int cl = atoi(clStr.c_str());

	std::cout << "clStr: '" << clStr << "'" << std::endl;
	std::cout << "cl: '" << cl << "'" << std::endl;

	return (cl);	
}

bool Client::receivedRequestLine() const {
	return (_buffer.find("\r\n") != std::string::npos);
}


bool Client::receivedHeaders() const {
	return (_buffer.find("\r\n\r\n") != std::string::npos);
}

/**
 * @throws
 */
void Client::parseRequest() {
	if (!receivedRequestLine())
		return;
	if (!_request.parsedRequestLine())
		_request.parseRequestLine(_buffer);

	if (!receivedHeaders()) {
		std::cout << "Headers not fully received yet" << std::endl;
		return;
	}
	if (!_request.parsedHeaders())
		_request.parseHeaders(_buffer);
	
	int contentLength = 0;
	try {
		std::string contentLengthStr = _request.getHeaderValue("Content-Length");
		contentLength = std::atoi(contentLengthStr.c_str());
	} catch (...) {}

	if (contentLength > 0) {
		if (!receivedBody(contentLength))
			return;
		if (!_request.parsedBody())
			_request.parseBody(_buffer, contentLength);
	}
	_status = READY;
}

bool Client::receivedBody(int contentLength) const {
	size_t crlfPos = _buffer.find("\r\n\r\n");
	if (crlfPos == std::string::npos)
		return (false);

	std::cout << _buffer.size() - (crlfPos + 4) << " < " << contentLength << std::endl;

	if (_buffer.size() - (crlfPos + 4) < (size_t)contentLength)
		return (false);

	return (true);
}

void Client::resetForNextRequest() {
	this->clearBuffer();
	this->setStatus(WAITING);
	_request = Request();
}

Request& Client::getRequest() {
	return _request;
}



