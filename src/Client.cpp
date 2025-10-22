/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 10:08:09 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/22 18:27:47 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "epoll.hpp"
#include "limits.h"

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
 * Client max body size is either specified in the config file
 * location, in the config file (out of a location scope), or the
 * default for a server: 1MB
 */
long long getMaxBodySize(const Location& loc, const Server& serv)
{
	long long	defaultV = ONE_MB;
	if (loc.getClientMaxBodySize() != -1)
		return loc.getClientMaxBodySize();
	if (serv.getClientMaxBodySize() != -1)
		return serv.getClientMaxBodySize();
	return defaultV;
}

/**
 * @throws if Content-Length is negative
 * @throws if Content-Length is not a number
 * @throws if Content-Length is larger than the allowed max body size
 */
size_t Client::checkAndGetContentLength(Server& serv, const std::string& contentLengthStr) {
	if (contentLengthStr.find_first_not_of("0123456789") != std::string::npos)
		throw BadHeaderValueException(contentLengthStr);
	long long contentLength = std::strtoll(contentLengthStr.c_str(), NULL, 10);
	if (contentLength == LONG_LONG_MAX || contentLength == LONG_LONG_MIN)
		throw BadHeaderValueException(contentLengthStr);
	if (contentLength < 0)
		throw BadHeaderValueException(contentLengthStr);
	Location loc = MethodExecutor::getRequestLocation(_request, serv);
	if (contentLength > getMaxBodySize(loc, serv))
		throw ContentTooLargeException();
	return contentLength;
}

/**
 * @throws
 * @note sets the client status to READY when the full request has been received / parsed
 */
void Client::parseRequest(Server& serv) {
	if (!receivedRequestLine())
		return;
	if (!_request.parsedRequestLine()) {
		_request.parseRequestLine(_recvBuffer);
	}

	if (!receivedHeaders())
		return;
	if (!_request.parsedHeaders()) {
		_request.parseHeaders(_recvBuffer);
	}
	
	std::string contentLengthStr = _request.getHeaderValue("Content-Length");
	if (contentLengthStr.empty()) {
		_status = READY;
		return;
	}

	size_t contentLength = this->checkAndGetContentLength(serv, contentLengthStr);
	if (!receivedBody(contentLength))
		return;
	if (!_request.parsedBody())
		_request.parseBody(_recvBuffer, contentLength);
	_status = READY;
}

/**
 * @return true if the body has been completely received.
 * @note checks from the end of headers ("\r\n\r\n") to contentLength bytes have been received
 */
bool Client::receivedBody(size_t contentLength) const {
	size_t crlfPos = _recvBuffer.find("\r\n\r\n");
	if (crlfPos == std::string::npos)
		return (false);

	size_t bodySize = _recvBuffer.size() - (crlfPos + 4);

	if (bodySize < contentLength)
		return (false);

	return (true);
}

/**
 * Each time we send a request, the client needs to reset states for next ones.
 */
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

/**
 * @brief Sends to the client (in multiple parts if needed) the pending response queued in _sendBuffer.
 */
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
		std::cout << "_sendBuffer completely sent" << std::endl;
		if (my_epoll_ctl(epollfd, EPOLL_CTL_MOD, EPOLLIN | EPOLLET, _fd) == -1) {
			return (EXIT_FAILURE);
		}
		this->resetForNextRequest();
	}
	return (EXIT_SUCCESS);
}





