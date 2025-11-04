/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsePacketExceptions.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 10:24:54 by mniemaz           #+#    #+#             */
/*   Updated: 2025/11/04 11:36:43 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSEPACKETEXCEPTIONS_HPP
#define PARSEPACKETEXCEPTIONS_HPP

#include "CodeDefines.h"
#include <exception>
#include <iostream>

class ParsePacketException : public std::exception {
protected:
	int _code;

public:
	ParsePacketException();
	ParsePacketException(int code);
	virtual ~ParsePacketException() throw();
	int getCode() const;
	virtual const char *what() const throw() = 0;
};

class RequestLineException : public ParsePacketException {
public:
	virtual const char *what() const throw();
};
class NoHeaderValueException : public ParsePacketException {
private:
	std::string _message;

public:
	virtual const char *what() const throw();
	NoHeaderValueException(const std::string &key);
	~NoHeaderValueException() throw();
};

class BadHeaderNameException : public ParsePacketException {
private:
	std::string _message;

public:
	virtual const char *what() const throw();
	BadHeaderNameException(const std::string &key);
	~BadHeaderNameException() throw();
};

class BadHeaderValueException : public ParsePacketException {
private:
	std::string _message;

public:
	virtual const char *what() const throw();
	BadHeaderValueException(const std::string &value);
	~BadHeaderValueException() throw();
};

class NoHeaderColumnException : public ParsePacketException {
public:
	virtual const char *what() const throw();
};

class UriTooLongException : public ParsePacketException {
public:
	UriTooLongException();
	virtual const char *what() const throw();
	~UriTooLongException() throw();
};

class ContentTooLargeException : public ParsePacketException {
public:
	ContentTooLargeException();
	virtual const char *what() const throw();
	~ContentTooLargeException() throw();
};

class MethodNotAllowedException : public ParsePacketException {
public:
	MethodNotAllowedException();
	virtual const char *what() const throw();
	~MethodNotAllowedException() throw();
};

class PageNotFoundException : public ParsePacketException {
public:
	PageNotFoundException();
	virtual const char *what() const throw();
	~PageNotFoundException() throw();
};

class HttpVersionNotSupportedException : public ParsePacketException {
public:
	HttpVersionNotSupportedException();
	virtual const char *what() const throw();
	~HttpVersionNotSupportedException() throw();
};

class MalformedChunkException : public ParsePacketException {
	public:
		virtual const char* what() const throw();
};

class TransferEncodingAndContentLengthException : public ParsePacketException {
	public:
		virtual const char* what() const throw();
};

class TransferCodingNotImplemented : public ParsePacketException {
	private:
		std::string _message;
	public:
		TransferCodingNotImplemented(const std::string& val);
		virtual const char* what() const throw();
		~TransferCodingNotImplemented() throw();
};

#endif