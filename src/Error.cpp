/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:36:13 by dmazari           #+#    #+#             */
/*   Updated: 2025/10/29 11:25:47 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Error.hpp"
# include "FtString.hpp"

Error::CanNotOpenFile::CanNotOpenFile(const std::string& message) {
	_message = "Can't open the file: " + message + ".";
}

Error::CanNotOpenFile::~CanNotOpenFile() throw() {}

Error::NoPageFound::NoPageFound(const std::string& message) {
	_message = "Can't find the page: " + message + ". As the 404 page not found.";
}

Error::NoPageFound::~NoPageFound() throw() {}

Error::DidNotFindSemicolon::DidNotFindSemicolon(const std::string& message) {
	_message = "Following '" + message + "' any semicolon was found.";
}

Error::DidNotFindSemicolon::~DidNotFindSemicolon() throw() {}

Error::UnknownToken::UnknownToken(const std::string& message) {
	_message = "This: '" + message + "' wasn't a value expected.";
}
	
Error::UnknownToken::~UnknownToken() throw() {}

Error::IntExpected::IntExpected(const std::string& message) {
	_message = "An int was expected and got: '" + message + "'.";
}

Error::IntExpected::~IntExpected() throw() {}

const char* Error::CanNotOpenFile::what() const throw() {
	if (_message.empty())
		return "Can't open file.";
	return _message.c_str();
}

const char* Error::ErrorBracketParseFile::what() const throw() {
	return "Not all brackets are closed or previously opened.";
}

const char*	Error::NoPageFound::what() const throw() {
	if (_message.empty())
		return "Page and error page not found.";
	return _message.c_str();
}

const char* Error::DidNotFindSemicolon::what() const throw() {
	if (_message.empty())
		return "Didn't find the Semicolon ';' after an argument in the ConfigFile.";
	return _message.c_str();
}

const char* Error::UnknownToken::what() const throw() {
	if (_message.empty())
		return "An other value was expected.";
	return _message.c_str();
}

const char* Error::IntExpected::what() const throw() {
	if (_message.empty())
		return "An int was expected.";
	return _message.c_str();
}

const char* Error::NoServerInConfigFile::what() const throw() {
	return "Any server was found in the config file.";
}

Error::NoRelatedServerFound::NoRelatedServerFound(int fd) {
	_message = "No related server found for the fd: " + FtString::my_to_string(fd) + ".";
}

Error::NoRelatedServerFound::~NoRelatedServerFound() throw() {}

const char* Error::NoRelatedServerFound::what() const throw() {
	if (_message.empty())
		return "No related server found.";
	return _message.c_str();
}
