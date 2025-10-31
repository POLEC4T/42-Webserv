/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:19:40 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/31 12:49:59 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"
#include "Error.hpp"

Context::Context() : _epollfd(-1) {}

Context::~Context() {
	if (_epollfd != -1)
		close(_epollfd);
}

//Getter
bool					Context::isRunningCgi(int fd) {
	std::map<int, t_Cgi>::iterator it = _mapRunningCgi.find(fd);
	if (it == _mapRunningCgi.end())
		return false;
	return true;
}

t_Cgi&					Context::getRunningCgi(int fd) {
	return _mapRunningCgi[fd];
}

std::vector<Server>&	Context::getServers() {
	return _servers;
}

const std::map<int, ErrorPage> &Context::getMapDefaultErrorPage() const {
	return _mapDefaultErrorPage;
}

int	Context::getEpollFd() const {
	return _epollfd;
}

// Setter
void	Context::addCgi(t_Cgi& cgi) {
	_mapRunningCgi[cgi.fd] = cgi;
}

void Context::addServer(const Server &server) { _servers.push_back(server); }

void	Context::setEpollFd(int fd) {
	_epollfd = fd;
}


// functions
// std::string	Context::checkTimedOutCgi() {
// 	for (std::map<int, t_Cgi>::iterator it = _mapRunningCgi.begin(); it != _mapRunningCgi.end(); ++it) {
// 		t_Cgi cgi = it->second;
// 		if (time() - cgi.startTime >= cgi.timeOut) {
// 			kill(cgi.pid);
// 			waitpid
// 		}
// 	}
// }

int getContent(std::string fileName, std::string &content, char separator) {
	std::string line;
	size_t i;
	size_t foundComment;
	std::ifstream file(fileName.c_str(), std::ios_base::in);

	if (!file.is_open())
		throw(Error::CanNotOpenFile(fileName));
	while (getline(file, line)) {
		if (!line.empty()) {
			for (i = 0; i < line.length() && iswspace(line[i]); i++) {
			}
			if (line[i] != '#') {
				foundComment = line.find('#');
				if (foundComment != std::string::npos)
					line.erase(foundComment, line.size());
				content += line;
				content += separator;
			}
		}
	}
	file.close();
	return 0;
}

void addSpace(std::string &content, char toSeparate) {
	for (size_t i = 0; i < content.length(); ++i) {
		if (content[i] == toSeparate) {
			content.insert(i, " ");
			++i;
		}
	}
}

void Context::parseAndAddServer(std::vector<std::string>::iterator &it,
								const std::vector<std::string>::iterator &itEnd,
								std::map<int, ErrorPage> errorPages) {
	int isClosed = 0;
	Server newServer(errorPages);

	while (it != itEnd) {
		if (*it == "{")
			isClosed++;
		else if (*it == "}")
			isClosed--;
		else if (*it == "host") {
			++it;
			if (it != itEnd) {
				newServer.setHost(*it);
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it == "port") {
			++it;
			while (it != itEnd && *it != ";") {
				newServer.addPort(*it);
				++it;
			}
			if (it == itEnd || *it != ";")
				throw(Error::DidNotFindSemicolon(*(it - 1)));
		} else if (*it == "client_max_body_size") {
			++it;
			if (it != itEnd)
				newServer.setClientMaxBodySize(*it);
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it == "timed_out") {
			++it;
			if (it != itEnd)
				newServer.setTimeOut(*it);
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it == "server_name") {
			++it;
			while (it != itEnd && *it != ";") {
				newServer.addName(*it);
				++it;
			}
			if (it == itEnd || *it != ";")
				throw(Error::DidNotFindSemicolon(*(it - 1)));
		} else if (*it == "error_page") {
			if ((it + 1) != itEnd && (it + 2) != itEnd) {
				newServer.addErrorPage(*(it + 1), *(it + 2));
				it += 2;
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw(Error::DidNotFindSemicolon(*it));
		} else if (*it == "location") {
			++it;
			if (it != itEnd)
				newServer.parseAndAddLocation(it, itEnd);
		} else if (*it != ";")
			throw(Error::UnknownToken(*it));
		if (isClosed == 0)
			break;
		++it;
	}
	if (isClosed != 0)
		throw(Error::ErrorBracketParseFile());
	addServer(newServer);
}

void Context::configFileParser(const std::string &fileName,
							std::map<int, ErrorPage> errorPages) {
	FtString content;
	std::vector<std::string> tokens;

	getContent(fileName, content, ' ');
	addSpace(content, ';');
	tokens = content.ft_split(" \n\b\t\r\v\f");
	for (std::vector<std::string>::iterator it = tokens.begin();
		it != tokens.end(); it++) {
		if (*it == "server")
			parseAndAddServer(++it, tokens.end(), errorPages);
	}
	if (_servers.empty())
		throw(Error::NoServerInConfigFile());
}

void	Context::parseAndSetMapDefaultErrorPage() {
    std::string	fileName = "serverData/errorPages/default/error_";
	std::vector<int>			codes;
	codes.push_back(BAD_REQUEST);
	codes.push_back(FORBIDDEN);
	codes.push_back(PAGE_NOT_FOUND);
	codes.push_back(METHOD_NOT_ALLOWED);
	codes.push_back(REQUEST_TIMEOUT);
	codes.push_back(CONTENT_TOO_LARGE);
	codes.push_back(URI_TOO_LONG);
	codes.push_back(INTERNAL_SERVER_ERROR);
	codes.push_back(NOT_IMPLEMENTED);
	codes.push_back(HTTP_VERSION_NOT_SUPPORTED);

	for (size_t i = 0; i < codes.size(); i++) {
		std::string	content;
		std::string codeStr = FtString::my_to_string<int>(codes[i]);
		std::string	name = "error_" + codeStr;


		getContent(fileName + codeStr + ".html", content, '\n');
		ErrorPage errorPage(name, content, codes[i]);
		_mapDefaultErrorPage[codes[i]] = errorPage;
	}
}

/**
 * @return true if fd is one of the listening sockets of any server
 */
bool	Context::isListenerFd(int fd) const {
	std::vector<Server>::const_iterator it;
	for (it = _servers.begin(); it != _servers.end(); ++it) {
		if (it->isListener(fd))
			return (true);
	}
	return (false);
}

/**
 * This fd must be either a listening socket or a client socket
 * @throw Error::NoRelatedServersFound if no server is found for this fd
 */
Server&	Context::getRelatedServer(int fd) {
	std::vector<Server>::iterator servIt;
	std::vector<int> sockfds;
	std::vector<Client> clients;

	for (servIt = _servers.begin(); servIt < _servers.end(); servIt++) {
		if (servIt->isClient(fd) || servIt->isListener(fd))
			return (*servIt);
	}
	throw (Error::NoRelatedServerFound(fd));
}
