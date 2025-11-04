/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 10:08:09 by mniemaz           #+#    #+#             */
/*   Updated: 2025/11/04 14:36:40 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "epoll.hpp"
#include "limits.h"
#include "ParsePacketExceptions.hpp"
#include "FtString.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "Error.hpp"
#include "MethodExecutor.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include "defines.h"
#include <string.h>
#include "Error.hpp"

#define ONE_MB 1048576

Client::Client() :
_sentIdx(0),
_status(WAITING),
_fd(-1),
_currChunkPart(SIZE),
_parsedChunksIdx(0),
_maxBodySize(0),
_contentLength(0),
_deleteAfterResponse(false)
{};

Client::Client(int fd) :
_sentIdx(0),
_status(WAITING),
_fd(fd),
_currChunkPart(SIZE),
_parsedChunksIdx(0),
_maxBodySize(0),
_contentLength(0),
_deleteAfterResponse(false)
{};

Client::~Client(){};

int 	Client::getFd() const {
	return (_fd);
}

/**
 * The status is set either WAITING or READY.
 * WAITING means the client did not receive the full request.
 * READY means the client received the full request and is therefore
 * ready to send the response
 */
t_client_status		Client::getStatus() const {
	return (_status);
}

const std::string&	Client::getRecvBuffer() const {
	return (_recvBuffer);
}

bool Client::getDeleteAfterResponse() const {
	return (_deleteAfterResponse);
}


void Client::setSendBuffer(const std::string& buf) {
	_sendBuffer = buf;
}

void Client::setStatus(t_client_status status) {
	_status = status;
}

void Client::setDeleteAfterResponse(bool b) {
	_deleteAfterResponse = b;
}


/**
 * @return true if the request line has been completely received.
 * @note checks for the presence of "\r\n" in the received buffer
 */
bool Client::receivedRequestLine() const {
	return (_recvBuffer.find("\r\n") != std::string::npos);
}

/**
 * @return true if the headers have been completely received.
 * @note checks for the presence of "\r\n\r\n" in the received buffer
 */
bool Client::receivedHeaders() const {
	return (_recvBuffer.find("\r\n\r\n") != std::string::npos);
}

/**
 * Client max body size is either specified in the config file
 * location, in the config file (out of a location scope), or the
 * default for a server: 1MB
 */
long long getMaxBodySize(const Location &loc, const Server &serv) {
	long long defaultV = ONE_MB;
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
size_t Client::_checkAndGetContentLength(const std::string& contentLengthStr) {
	if (contentLengthStr.find_first_not_of("0123456789") != std::string::npos)
		throw BadHeaderValueException(contentLengthStr);
	_contentLength = std::strtoll(contentLengthStr.c_str(), NULL, 10);
	if (_contentLength == LONG_LONG_MAX || _contentLength == LONG_LONG_MIN)
		throw BadHeaderValueException(contentLengthStr);
	if (_contentLength < 0)
		throw BadHeaderValueException(contentLengthStr);
	
	if (PRINT)
		std::cout << "Max body size allowed: " << _maxBodySize << std::endl << "Content-Length received: " << _contentLength << std::endl;
	if (_contentLength > _maxBodySize)
		throw ContentTooLargeException();
	return _contentLength;
}

/**
 * @throws MalformedChunkException
 * @return size_t value of a hexadecimal string
 */
size_t ft_atoi_hexa(const std::string& str) {
	std::istringstream iss(str);
	size_t res;
	iss >> std::hex >> res;
	if (iss.fail() || !iss.eof()) {
		std::cout << "Invalid chunk size: " << str << std::endl;
		throw MalformedChunkException();
	}
	return res;
}

/**
 * @throws MalformedChunkException
 * @return true if the full body has been received
 * 
 * @example chunked body:
 * 5\r\n		-> chunk size line
 * Hello\r\n 	-> chunk data line
 * 7\r\n
 * , World\r\n
 * 0\r\n
 * \r\n
 */
bool Client::unchunkBody(const std::string& chunks) {
	size_t pos = 0;
	
	while (1) {
		if (_currChunkPart == SIZE) {
			t_chunk_state state = _parseChunkSize(chunks, pos);
			if (state == CHUNK_FINAL_COMPLETE)
				return (true);
			if (state == CHUNK_INCOMPLETE)
				return (false);
		}

		if (_currChunkPart == DATA) {
			if (_parseChunkData(chunks, pos) == CHUNK_INCOMPLETE)
				return (false);
		}
	}

	return (true);
}

/**
 * @throws MalformedChunkException
 * @throws ContentTooLargeException
 * @return true if the full body has been received
 * 
 */
Client::t_chunk_state Client::_parseChunkSize(const std::string& chunks, size_t& pos)
{
	size_t startline;

	startline = _parsedChunksIdx;
	pos = chunks.find("\r\n", _parsedChunksIdx);
	if (pos == std::string::npos) {
		if (PRINT)
			std::cout << "Expected CRLF after chunk size" << std::endl;
		return CHUNK_INCOMPLETE;
	}

	FtString chunkSizeStr = chunks.substr(startline, pos - startline);
	size_t semi = chunkSizeStr.find(';', startline);
	if (semi != std::string::npos)
		chunkSizeStr = chunkSizeStr.substr(0, semi);
	chunkSizeStr.trim();

	if (chunkSizeStr.empty()) {
		std::cout << "Chunk size line is empty" << std::endl;
		throw MalformedChunkException();
	}

	_currChunkSize = ft_atoi_hexa(chunkSizeStr);

	if (_currChunkSize == 0)
	{
		if (chunks.compare(pos, CRLF_SIZE * 2, "\r\n\r\n") != 0) {
			if (PRINT)
				std::cout << "Expected double CRLF after final zero" << std::endl;
			return CHUNK_INCOMPLETE;
		}
		return CHUNK_FINAL_COMPLETE;
	}
	else if ((long long)(_request.getBody().size() + _currChunkSize) > _maxBodySize) {
		throw ContentTooLargeException();
	}
	pos += CRLF_SIZE; // CRLF of the end of chunk size no need to check its presence as we found it before
	_parsedChunksIdx = pos;
	_currChunkPart = DATA;
	return CHUNK_COMPLETE;
	
}

/**
 * @return CHUNK_COMPLETE if full chunk data line has been received
 */
Client::t_chunk_state Client::_parseChunkData(const std::string& chunks, size_t& pos)
{
	pos = _parsedChunksIdx;
	if (pos + _currChunkSize > chunks.size()) {
		if (PRINT)
			std::cout << "Expected more chunk data" << std::endl;
		return CHUNK_INCOMPLETE;
	}

	if (chunks.substr(pos + _currChunkSize, CRLF_SIZE) != "\r\n") {
		if (PRINT)
			std::cout << "Expected CRLF after chunk data" << std::endl;
		return CHUNK_INCOMPLETE;
	}
	_request.appendBody(chunks.substr(pos, _currChunkSize));
	pos += (_currChunkSize + CRLF_SIZE);
	_parsedChunksIdx = pos;
	_currChunkPart = SIZE;
	return CHUNK_COMPLETE;
}

/**
 * @throws TransferEncodingAndContentLengthException
 * @throws ContentTooLargeException
 * @note sets the client status to READY when the full request has been received / parsed
 */
void Client::parsePacket(Server& serv) {
	static std::string contentLengthStr;
	static std::string transferEncodingStr;

	if (!receivedRequestLine())
		return;
	if (!_request.parsedRequestLine())
		_request.parseRequestLine(_recvBuffer);

	if (!receivedHeaders())
		return;
	if (!_request.parsedHeaders()) {
		_request.parseHeaders(_recvBuffer);
		_maxBodySize = getMaxBodySize(MethodExecutor::getRequestLocation(_request, serv), serv);
		contentLengthStr = _request.getHeaderValue("Content-Length");
		transferEncodingStr = _request.getHeaderValue("Transfer-Encoding");
		if (!contentLengthStr.empty() && !transferEncodingStr.empty()) {
			throw TransferEncodingAndContentLengthException();
		} else if (contentLengthStr.empty() && transferEncodingStr.empty()) {
			_status = READY;
			return;
		} else if (!contentLengthStr.empty()) {
			_contentLength = _checkAndGetContentLength(contentLengthStr);
		} else /* if (!transferEncodingStr.empty()) */ {
			if (transferEncodingStr != "chunked")
				throw TransferCodingNotImplemented(transferEncodingStr);
		}
	}
	// get to this line means either Content-Length or Transfer-Encoding: chunked is present
	
	size_t startBody = _recvBuffer.find("\r\n\r\n") + 4;
	if (!contentLengthStr.empty()) {
		if (!receivedBody(_contentLength))
			return;
		_request.appendBody(_recvBuffer.substr(startBody, _contentLength));
		_status = READY;
	}
	else /* if (!transferEncodingStr.empty()) */ {
		if (this->unchunkBody(FtString(_recvBuffer.substr(startBody))))
			_status = READY;
	}


}

/**
 * @return true if the body has been completely received.
 * @note checks from the end of headers ("\r\n\r\n") to contentLength bytes have
 * been received
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
 * Each time we send a request, the client needs to reset states for next request.
 */
void Client::resetForNextRequest() {
	_request = Request();
	_status = WAITING;
	_recvBuffer.clear();
	_sendBuffer.clear();
	_chunks.clear();
	_currChunkPart = SIZE;
	_parsedChunksIdx = 0;
	_sentIdx = 0;
	_maxBodySize = 0;
	_contentLength = 0;
	_deleteAfterResponse = false;
}

Request &Client::getRequest() { return _request; }

/**
 * @brief Sends to the client (in multiple parts if needed) the pending response
 * queued in _sendBuffer.
 */
int Client::sendPendingResponse(int epollfd) {
	
	ssize_t sentlen = send(	_fd,
							_sendBuffer.c_str() + _sentIdx,
							_sendBuffer.size() - _sentIdx,
							0);

	if (sentlen == -1) {
		std::cerr << "send: " << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	}

	_sentIdx += sentlen;

	if (_sentIdx >= _sendBuffer.size()) {
		if (PRINT)
			std::cout << "_sendBuffer completely sent" << std::endl;
		if (_deleteAfterResponse)
			return (EXIT_FAILURE);
		if (my_epoll_ctl(epollfd, EPOLL_CTL_MOD, EPOLLIN, _fd) == -1) {
			return (EXIT_FAILURE);
		}
		this->resetForNextRequest();
	}
	return (EXIT_SUCCESS);
}


int Client::readPacket() {
	char buffer[MAX_RECV] = { 0 };
	ssize_t sizeRead;

	sizeRead = recv(_fd, buffer, MAX_RECV, 0);
	if (sizeRead == -1) {
		std::cerr << "recv:" << strerror(errno) << std::endl;
		return (EXIT_FAILURE);
	} else if (sizeRead == 0) {
		if (PRINT)
			std::cout << "Client disconnected, fd: " << _fd << std::endl;
		return (EXIT_FAILURE);
	}
	if (_recvBuffer.empty())
		_request.setStartTime(time(NULL));
	_recvBuffer.append(buffer, sizeRead);
	return (EXIT_SUCCESS);
}
