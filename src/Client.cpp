/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 10:08:09 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/15 14:57:15 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _status(READY), _fd(-1) {};

Client::Client(int fd) : _status(READY), _fd(fd) {};

Client::~Client() {};

int					Client::getFd() const {
	return (_fd);
}

const std::string&	Client::getBuffer() const {
	return (_buffer);
}

t_client_status		Client::getStatus() const {
	return (_status);
}

void				Client::setStatus(t_client_status status) {
	_status = status;
}


void				Client::appendBuffer(char *buffer) {
	_buffer.append(buffer);
}

void				Client::appendBuffer(const char *buffer) {
	_buffer.append(buffer);
}

void				Client::clearBuffer() {
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

bool				Client::hasReceivedFullReq() {
	size_t crlfPos = _buffer.find("\r\n\r\n");
	if (crlfPos == std::string::npos)
		return (false);

	int contentLength = _getContentLength();
	std::cout << "contentLength: " << contentLength << std::endl;
	if (contentLength == -1)
		return (true);

	if (_buffer.size() - (crlfPos + 4) < (size_t)contentLength)
		return (false);

	return (true);
}





