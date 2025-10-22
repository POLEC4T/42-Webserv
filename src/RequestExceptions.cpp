/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestExceptions.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 10:27:07 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/22 12:20:18 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestExceptions.hpp"

RequestException::RequestException() : _code(400) {}

RequestException::RequestException(int code) : _code(code) {}

int RequestException::getCode() const {
	return _code;
}

RequestException::~RequestException() throw() {}



const char* NoHeaderValueException::what() const throw() {
	if (_message.empty())
		return "Request: No header field for this key";
	
	return _message.c_str();
}

NoHeaderValueException::NoHeaderValueException(const std::string& missingKey) {
	_message = "Request: No header field for \"" + missingKey + "\"";
}

NoHeaderValueException::~NoHeaderValueException() throw() {}

const char* RequestLineException::what() const throw() {
	return "Request: Error in the request line.";
}

const char* NoHeaderColumnException::what() const throw() {
	return "Request: A header column is missing.";
}

const char* BadHeaderNameException::what() const throw() {
	if (_message.empty())
		return "Request: bad header name";
	return _message.c_str();
}

BadHeaderNameException::BadHeaderNameException(const std::string& headerName) {
	if (headerName.empty())
		_message = "Request: bad header name: empty";
	else
	_message = "Request: bad header name: \"" + headerName + "\"";
}

BadHeaderNameException::~BadHeaderNameException() throw() {}

UriTooLongException::UriTooLongException() : RequestException(URI_TOO_LONG) {}

UriTooLongException::~UriTooLongException() throw() {}

const char* UriTooLongException::what() const throw() {
	return "Request: URI too long.";
}

const char* BadHeaderValueException::what() const throw() {
	if (_message.empty())
		return "Request: bad header value";
	return _message.c_str();
}

BadHeaderValueException::BadHeaderValueException(const std::string& headerValue) {
	if (headerValue.empty())
		_message = "Request: bad header value: empty";
	else
		_message = "Request: bad header value: \"" + headerValue + "\"";
}

BadHeaderValueException::~BadHeaderValueException() throw() {}

HttpVersionNotSupportedException::HttpVersionNotSupportedException() : RequestException(HTTP_VERSION_NOT_SUPPORTED) {}

HttpVersionNotSupportedException::~HttpVersionNotSupportedException() throw() {}

const char* HttpVersionNotSupportedException::what() const throw() {
	return "Request: HTTP version not supported.";
}