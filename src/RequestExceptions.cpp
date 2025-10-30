/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestExceptions.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 10:27:07 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/30 11:15:33 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestExceptions.hpp"

RequestException::RequestException() : _code(400) {}

RequestException::RequestException(int code) : _code(code) {}

int RequestException::getCode() const { return _code; }

RequestException::~RequestException() throw() {}

const char *NoHeaderValueException::what() const throw() {
	if (_message.empty())
		return "Request: No header field for this key";

	return _message.c_str();
}

NoHeaderValueException::NoHeaderValueException(const std::string &missingKey) {
	_message = "Request: No header field for \"" + missingKey + "\"";
}

NoHeaderValueException::~NoHeaderValueException() throw() {}

const char *RequestLineException::what() const throw() {
	return "Request: Error in the request line.";
}

const char *NoHeaderColumnException::what() const throw() {
	return "Request: A header column is missing.";
}

const char *BadHeaderNameException::what() const throw() {
	if (_message.empty())
		return "Request: bad header name";
	return _message.c_str();
}

BadHeaderNameException::BadHeaderNameException(const std::string &headerName) {
	if (headerName.empty())
		_message = "Request: bad header name: empty";
	else
		_message = "Request: bad header name: \"" + headerName + "\"";
}

BadHeaderNameException::~BadHeaderNameException() throw() {}

UriTooLongException::UriTooLongException() : RequestException(URI_TOO_LONG) {}

UriTooLongException::~UriTooLongException() throw() {}

const char *UriTooLongException::what() const throw() {
	return "Request: URI too long.";
}

ContentTooLargeException::ContentTooLargeException()
	: RequestException(CONTENT_TOO_LARGE) {}

ContentTooLargeException::~ContentTooLargeException() throw() {}

const char *ContentTooLargeException::what() const throw() {
	return "Request: Content too large";
}

MethodNotAllowedException::MethodNotAllowedException()
	: RequestException(METHOD_NOT_ALLOWED) {}

MethodNotAllowedException::~MethodNotAllowedException() throw() {}

const char *MethodNotAllowedException::what() const throw() {
	return "Request: Method not allowed";
}

PageNotFoundException::PageNotFoundException()
	: RequestException(PAGE_NOT_FOUND) {}

PageNotFoundException::~PageNotFoundException() throw() {}

const char *PageNotFoundException::what() const throw() {
	return "Request: Page not found";
}

const char *BadHeaderValueException::what() const throw() {
	if (_message.empty())
		return "Request: bad header value";
	return _message.c_str();
}

BadHeaderValueException::BadHeaderValueException(
	const std::string &headerValue) {
	if (headerValue.empty())
		_message = "Request: bad header value: empty";
	else
		_message = "Request: bad header value: \"" + headerValue + "\"";
}

BadHeaderValueException::~BadHeaderValueException() throw() {}

HttpVersionNotSupportedException::HttpVersionNotSupportedException()
	: RequestException(HTTP_VERSION_NOT_SUPPORTED) {}

HttpVersionNotSupportedException::~HttpVersionNotSupportedException() throw() {}

const char *HttpVersionNotSupportedException::what() const throw() {
	return "Request: HTTP version not supported.";
}

const char* MalformedChunkException::what() const throw() {
	return "Request: Malformed chunk in chunked transfer encoding.";
}

const char* TransferEncodingAndContentLengthException::what() const throw() {
	return "Request: Transfer-Encoding and Content-Length are in headers.";
}

TransferCodingNotImplemented::TransferCodingNotImplemented(const std::string& val) : RequestException(NOT_IMPLEMENTED), _message("Request: Transfer coding not implemented: " + val) {}

TransferCodingNotImplemented::~TransferCodingNotImplemented() throw() {}

const char* TransferCodingNotImplemented::what() const throw() {
	return _message.c_str();
}
