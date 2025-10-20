/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 10:08:09 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/20 20:49:58 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _sentIdx(0), _status(WAITING), _fd(-1) {};

Client::Client(int fd) : _sentIdx(0), _status(WAITING), _fd(fd){};

Client::~Client() {};

int 	Client::getFd() const {
	return (_fd);
}

const std::string&	Client::getBuffer() const {
	return (_recvBuffer);
}

t_client_status		Client::getStatus() const {
	return (_status);
}

void Client::setSendBuffer(const std::string& buf) {
	_sendBuffer = buf;
}


void Client::setStatus(t_client_status status) {
	_status = status;
}


void Client::appendBuffer(char *buffer) {
	_recvBuffer.append(buffer);
}

void Client::appendBuffer(const char *buffer) {
	_recvBuffer.append(buffer);
}

void Client::clearBuffer() {
	_recvBuffer.clear();
}

bool Client::receivedRequestLine() const {
	return (_recvBuffer.find("\r\n") != std::string::npos);
}

bool Client::receivedHeaders() const {
	return (_recvBuffer.find("\r\n\r\n") != std::string::npos);
}

/**
 * @throws
 */
void Client::parseRequest() {
	if (!receivedRequestLine())
		return;
	if (!_request.parsedRequestLine())
		_request.parseRequestLine(_recvBuffer);

	if (!receivedHeaders())
		return;
	if (!_request.parsedHeaders())
		_request.parseHeaders(_recvBuffer);
	
	int contentLength = 0;
	try {
		std::string contentLengthStr = _request.getHeaderValue("Content-Length");
		contentLength = std::atoi(contentLengthStr.c_str());
	} catch (...) {}

	if (contentLength > 0) {
		if (!receivedBody(contentLength))
			return;
		if (!_request.parsedBody())
			_request.parseBody(_recvBuffer, contentLength);
	}
	_status = READY;
}

bool Client::receivedBody(int contentLength) const {
	size_t crlfPos = _recvBuffer.find("\r\n\r\n");
	if (crlfPos == std::string::npos)
		return (false);

	size_t bodySize = _recvBuffer.size() - (crlfPos + 4);

	if (bodySize < (size_t)contentLength)
		return (false);

	return (true);
}

void Client::resetForNextRequest() {
	_recvBuffer.clear();
	_sendBuffer.clear();
	_status = WAITING;
	_sentIdx = 0;
	_request = Request();
}

Request& Client::getRequest() {
	return _request;
}

int Client::sendPendingResponse(int epollfd) {
	
	ssize_t sentlen = send(_fd,
		_sendBuffer.c_str() + _sentIdx,
		_sendBuffer.size() - _sentIdx,
		0);

	if (sentlen == -1) {
		std::cout << "send: " << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}

	_sentIdx += sentlen;

	if (_sentIdx >= _sendBuffer.size()) {
		if (my_epoll_ctl(epollfd, EPOLL_CTL_MOD, EPOLLIN | EPOLLET, _fd) == -1) {
			return (EXIT_FAILURE);
		}
		this->resetForNextRequest();
	}
	return (EXIT_SUCCESS);
}





