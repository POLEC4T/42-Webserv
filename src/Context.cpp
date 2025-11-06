/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:19:40 by mazakov           #+#    #+#             */
/*   Updated: 2025/11/06 14:24:27 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"
#include "Error.hpp"
#include "Response.hpp"
#include <string.h>
#include <sys/wait.h>
#include <algorithm>
#include "defines.h"

int		queueResponse(Client &client, std::string &response, int epollfd);
void	ftClose(int* fd);


Context::Context() : _epollfd(-1) {}

Context::~Context() {
	if (_epollfd != -1)
		close(_epollfd);

	int fd = -1;
	std::map<int, CGI>::iterator itMap;
	for (itMap = _mapRunningCGIs.begin(); itMap != _mapRunningCGIs.end(); ++itMap) {
		CGI &cgi = itMap->second;
		kill(cgi.getPid(), SIGKILL);
		waitpid(cgi.getPid(), NULL, 0);
		fd = cgi.getFd();
		if (fd != -1)
			close(fd);
	}
	_mapRunningCGIs.clear();
}

// Getter
/**
 * @return true if the eventfd is the fd of a CGI
 */
bool Context::isRunningCGI(int eventfd) {
	std::map<int, CGI>::iterator it = _mapRunningCGIs.find(eventfd);
	if (it == _mapRunningCGIs.end())
		return false;
	return true;
}

std::vector<Server> &Context::getServers() { return _servers; }

const std::map<int, ErrorPage> &Context::getMapDefaultErrorPage() const {
	return _mapDefaultErrorPage;
}

int Context::getEpollFd() const { return _epollfd; }

// Setter
void Context::addCgi(CGI &cgi) {
	_mapRunningCGIs.insert(std::make_pair(cgi.getFd(), cgi));
}

void Context::addServer(const Server &server) { _servers.push_back(server); }

void Context::setEpollFd(int fd) { _epollfd = fd; }

std::map<std::string, std::string>	parseTheHeaderOfCGI(std::string& header, int& code, std::string& status) {
	std::map<std::string, std::string> headerValues;
	FtString myString = header;
	std::vector<std::string> tokens = myString.ft_split_word("\r\n");
	if (tokens.size() == 1)
		tokens = myString.ft_split("\n");
	
	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
		FtString token = *it;
		std::vector<std::string> lineValues = token.ft_split(":");
		if (lineValues.size() < 2)
		{
			std::cerr << "Malformed header line in CGI response: " << *it << std::endl;
			return std::map<std::string, std::string>();
		}
		if (lineValues.at(0) == "Status") {
			FtString value = lineValues.at(1);
			std::vector<std::string> values = value.ft_split(" ");
			if (values.size() < 2)
			{
				std::cerr << "Invalid status line in CGI response: " << *it << std::endl;
				return std::map<std::string, std::string>();
			}
			std::istringstream iss(values[0]);
			iss >> code;
			if (iss.fail() || !iss.eof())
			{
				std::cerr << "Invalid status code in CGI response: " << values[0] << std::endl;
				return std::map<std::string, std::string>();
			}
			for (std::vector<std::string>::iterator it = (values.begin()) + 1; it != values.end(); ++it)
			{
				status += *it;
				if ((it + 1) != values.end())
					status += " ";
			}
		}
		else
			headerValues[lineValues[0]] = lineValues[1];
	}
	return headerValues;
}

std::string	buildingFullResponse(std::string& header, const std::string& body, Request& req) {
	int code;
	std::string status;
	std::map<std::string, std::string>	finalHeader = parseTheHeaderOfCGI(header, code, status);
	if (finalHeader.empty())
		return std::string();
	Response response = Response(req.getVersion(), code, status, body);
	for (std::map<std::string, std::string>::iterator it = finalHeader.begin(); it != finalHeader.end(); ++it) {
		response.setHeader(it->first, it->second);
	}
	return response.build();
}

int	parseTheOutputOfCGI(std::string& output, Request& req) {
	size_t statusPlace = output.find("Status:", 0);
	if (statusPlace == std::string::npos)
	{
		std::cerr << "No Status line in the CGI response" << std::endl;
		return (EXIT_FAILURE);
	}
	
	FtString	token = output;
	std::vector<std::string> tokens = token.ft_split_word("\r\n\r\n");
	if (tokens.size() == 1)
	{
		std::cerr << "There's no empty line in the CGI response" << std::endl;
		return (EXIT_FAILURE);
	}

	output = buildingFullResponse(tokens[0], tokens.size() >= 2 ? tokens[1] : "", req);
	if (output.empty())
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

void Context::handleEventCgi(int fd) {
	std::map<int, CGI>::iterator it = _mapRunningCGIs.find(fd);
	if (it == _mapRunningCGIs.end())
		return ;

	if (PRINT)
		std::cout << "Handling CGI event with fd: " << fd << std::endl;
	CGI &cgi = it->second;

	char buffer[MAX_RECV];
	int bytes = read(fd, buffer, MAX_RECV - 1);
	if (bytes == -1) {
		std::cerr << "read:" << strerror(errno) << std::endl;
		cgi.getServer().deleteClient(cgi.getClient().getFd());
		return ;
	} else if (bytes > 0) {
		buffer[bytes] = '\0';
		cgi.appendOutput(buffer);
	}

	int status;
	pid_t r = waitpid(cgi.getPid(), &status, WNOHANG);

	if (r == cgi.getPid()) {
		std::string response;
		if (parseTheOutputOfCGI(cgi.getOutput(), cgi.getRequest()) == EXIT_FAILURE) 
		{
			response = Response(cgi.getClient().getRequest().getVersion(),
			cgi.getServer().getErrorPageByCode(INTERNAL_SERVER_ERROR)).build();
		}
		else
			response = cgi.getOutput(); 
		if (queueResponse(cgi.getClient(), response, _epollfd) == EXIT_FAILURE) {
			cgi.getServer().deleteClient(cgi.getClient().getFd());
		}
		close(fd);
		_mapRunningCGIs.erase(fd);
	}
	return ;
}

/* The code snippet `PRINT) std::cout << "EPOLLIN fd: "` seems to be a typo or a mistake in
	the code. It looks like there is a missing opening parenthesis after `PRINT`. The correct
	syntax should be `if (PRINT) { std::cout << "EPOLLIN fd: "; }`. */
	

void Context::checkTimedOutCGI() {
	int 		now = time(NULL);
	int 		fd;
	std::string response;

	std::vector<int> 			CGIsToErase;
	std::map<int, CGI>::iterator itMap;

	for (itMap = _mapRunningCGIs.begin(); itMap != _mapRunningCGIs.end(); ++itMap) {
		CGI &cgi = itMap->second;
		if (PRINT)
			std::cout << "CGI time left: " << cgi.getTimeOutValue() - (now - cgi.getStartTime()) << std::endl;
		if (now - cgi.getStartTime() >= cgi.getTimeOutValue()) {
			
			fd = cgi.getFd();

			cgi.getClient().setdeleteClientAfterResponse(true);

			kill(cgi.getPid(), SIGKILL);
			if (waitpid(cgi.getPid(), NULL, 0) == -1) {
				std::cerr << "waitpid failed" << std::endl;
				response = Response(cgi.getClient().getRequest().getVersion(),
					cgi.getServer().getErrorPageByCode(INTERNAL_SERVER_ERROR)).build();
			} else {
				response = Response(cgi.getClient().getRequest().getVersion(),
					cgi.getServer().getErrorPageByCode(GATEWAY_TIMEOUT)).build();
			}
		
			if (queueResponse(cgi.getClient(), response, _epollfd) == EXIT_FAILURE)
				cgi.getServer().deleteClient(cgi.getClient().getFd());
			
			close(fd);
			CGIsToErase.push_back(fd);
		}
	}

	std::vector<int>::iterator itFd;

	for (itFd = CGIsToErase.begin(); itFd != CGIsToErase.end(); ++itFd)
	{
		if (PRINT)
			std::cout << "Erasing CGI with fd: " << *itFd << std::endl;
		_mapRunningCGIs.erase(*itFd);
	}
}

/**
 * @brief delete each client that must be. A client must be closed if it has timed out
 * or if it is in linger status and his linger deadline has been reached.
 */
void	Context::handleClientTimeoutsAndLinger() {
	std::map<int, Client>::iterator itcl;
	std::string response;
	std::vector<Server>::iterator itserv;
	std::map<Server*, std::vector<int> > toDelete;
	

	for (itserv = _servers.begin(); itserv < _servers.end(); ++itserv) {
		std::map<int, Client>& clients = itserv->getClients();
		for (itcl = clients.begin(); itcl != clients.end(); ++itcl) {
			Client& cl = itcl->second;
			if (cl.getStatus() == LINGER) {
				if (PRINT)
					std::cout << "Linger time left: " << cl.getLingerDeadline() - time(NULL)  << "s" << std::endl;
				if (time(NULL) >= cl.getLingerDeadline()) {
					std::cout << "Linger deadline exceeded, deleting client" << std::endl;
					toDelete[&(*itserv)].push_back(cl.getFd());
				}
			}
			else if (cl.getRecvBuffer().empty() || cl.getStatus() == READY)
				continue;
			else if (cl.getRequest().hasTimedOut(itserv->getTimedOutValue())) {
				std::cout << "client timed out: fd " << cl.getFd() << std::endl;
				response = Response(cl.getRequest().getVersion(), itserv->getErrorPageByCode(REQUEST_TIMEOUT)).build();
				cl.setdeleteClientAfterResponse(true);
				if (queueResponse(cl, response, _epollfd) == EXIT_FAILURE)
					toDelete[&(*itserv)].push_back(cl.getFd());
			}
		}
	}
	
	std::map<Server *, std::vector<int> >::iterator itpair;
	std::vector<int>::iterator itfd;
	for (itpair = toDelete.begin(); itpair != toDelete.end(); ++itpair) {
		Server *server = itpair->first;
		for (itfd = itpair->second.begin(); itfd != itpair->second.end(); ++itfd) {
			server->deleteClient(*itfd);
		}
	}
}


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

void Context::parseAndSetMapDefaultErrorPage() {
	std::string fileName = "serverData/errorPages/default/error_";
	std::vector<int> codes;
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
	codes.push_back(GATEWAY_TIMEOUT);

	for (size_t i = 0; i < codes.size(); i++) {
		std::string content;
		std::string codeStr = FtString::my_to_string<int>(codes[i]);
		std::string name = "error_" + codeStr;

		getContent(fileName + codeStr + ".html", content, '\n');
		ErrorPage errorPage(name, content, codes[i]);
		_mapDefaultErrorPage[codes[i]] = errorPage;
	}
}

/**
 * @return true if fd is one of the listening sockets of any server
 */
bool Context::isListenerFd(int fd) const {
	std::vector<Server>::const_iterator it;
	for (it = _servers.begin(); it != _servers.end(); ++it) {
		if (it->isListener(fd))
			return (true);
	}
	return (false);
}

/**
 * A server contains fds: listening sockets and client sockets. If the given fd
 * matches one of them, return the server.
 * @param fd the fd to find the related server
 * @return the server related to the fd, or NULL if not found
 */
Server* Context::getRelatedServer(int fd) {
	std::vector<Server>::iterator servIt;
	std::vector<int> sockfds;
	std::vector<Client> clients;

	for (servIt = _servers.begin(); servIt < _servers.end(); ++servIt) {
		if (servIt->isClient(fd) || servIt->isListener(fd))
			return &(*servIt);
	}
	return NULL;
}
