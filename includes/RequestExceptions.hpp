/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestExceptions.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 10:24:54 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/22 16:10:24 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTEXCEPTIONS_HPP
#define REQUESTEXCEPTIONS_HPP

#include <exception>
#include <iostream>
#include "CodeDefines.h"

class RequestException : public std::exception {
	protected:
		int _code;
	public:
		RequestException();
		RequestException(int code);
		virtual ~RequestException() throw();
		int getCode() const;
		virtual const char* what() const throw() = 0;

};

class RequestLineException : public RequestException {
	public:
		virtual const char* what() const throw();
};
class NoHeaderValueException : public RequestException {
	private:
		std::string _message;
	public:
		virtual const char* what() const throw();
		NoHeaderValueException(const std::string& key);
		~NoHeaderValueException() throw();
};

class BadHeaderNameException : public RequestException {
	private:
		std::string _message;
	public:
		virtual const char* what() const throw();
		BadHeaderNameException(const std::string& key);
		~BadHeaderNameException() throw();
};

class BadHeaderValueException : public RequestException {
	private:
		std::string _message;
	public:
		virtual const char* what() const throw();
		BadHeaderValueException(const std::string& value);
		~BadHeaderValueException() throw();
};

class NoHeaderColumnException : public RequestException {
	public:
		virtual const char* what() const throw();
};

class UriTooLongException : public RequestException {
	public:
		UriTooLongException();
		virtual const char* what() const throw();
		~UriTooLongException() throw();
};

class ContentTooLargeException : public RequestException {
	public:
		ContentTooLargeException();
		virtual const char* what() const throw();
		~ContentTooLargeException() throw();
};


class MethodNotAllowedException : public RequestException {
	public:
		MethodNotAllowedException();
		virtual const char* what() const throw();
		~MethodNotAllowedException() throw();
};

class PageNotFoundException : public RequestException {
	public:
		PageNotFoundException();
		virtual const char* what() const throw();
		~PageNotFoundException() throw();
};

class HttpVersionNotSupportedException : public RequestException {
	public:
		HttpVersionNotSupportedException();
		virtual const char* what() const throw();
		~HttpVersionNotSupportedException() throw();
};



#endif