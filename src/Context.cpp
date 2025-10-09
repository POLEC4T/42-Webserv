/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:19:40 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/09 15:07:13 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Context.hpp"

Context::Context() {}

Context::~Context() {}

Context::Context(const Context& cpy) {
	std::vector<Server>::const_iterator it = cpy._servers.begin();

	while (it != cpy._servers.end())
	{
		_servers.push_back(*it);
		it++;
	}
}

Context&	Context::operator=(const Context& other) {
	if (this != &other)
	{
		this->_servers.erase(_servers.begin(), _servers.end());
		std::vector<Server>::const_iterator it = other._servers.begin();
		while (it != other._servers.end())
		{
			_servers.push_back(*it);
			it++;
		}
	}
	return *this;
}

//Getter
const std::vector<Server>	Context::getServers() const {
	return _servers;
}

//Setter
void	Context::addServer(const Server& server) {
	_servers.push_back(server);
}

//functions
int	getContent(std::string fileName, std::string& content) {
	std::string		line;
	size_t			i;
	size_t			foundComment;
	std::ifstream	file(fileName.c_str(), std::ios_base::in);

	if (!file.is_open())
		return 1;
	while (getline(file, line)) {
		if (!line.empty())
		{
			for (i = 0; i < line.length() && iswspace(line[i]); i++) {}
			if (line[i] != '#')
			{
				foundComment = line.find('#');
				if (foundComment != std::string::npos)
					line.erase(foundComment, line.size());
				content += line;
				content += ' ';
			}
		}
	}
	file.close();
	return 0;
}

void	addSpace(std::string& content, char toSeparate) {
	for (size_t i = 0; i < content.length(); ++i) {
		if (content[i] == toSeparate) {
			content.insert(i, " ");
			++i;
		}
	}
}


void	Context::parseAndAddServer(std::vector<std::string>::iterator& it,
		const std::vector<std::string>::iterator& itEnd) {
	int isClosed = 0;
	Server newServer;

	while (it != itEnd)
	{
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
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "port") {
			++it;
			if (it != itEnd)
				newServer.setPort(*it);
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "client_max_body_size") {
			++it;
			if (it != itEnd)
				newServer.setClientMaxBodySize(*it);
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "server_name") {
			++it;
			while (it != itEnd && *it != ";") {
				newServer.addName(*it);
				++it;
			}
			if (it == itEnd || *it != ";")
				throw (Error::DidNotFindSemicolon(*(it - 1)));
		}
		else if (*it == "error_page") {
			if ((it + 1) != itEnd && (it + 2) != itEnd) {
				newServer.addErrorPage(*(it + 1), *(it + 2));
				it += 2;
			}
			if ((it + 1) == itEnd || *(it + 1) != ";")
				throw (Error::DidNotFindSemicolon(*it));
		}
		else if (*it == "location") {
			++it;
			if (it != itEnd)
				newServer.parseAndAddLocation(it, itEnd);
		}
		else if (*it != ";")
			throw (Error::UnknownToken(*it));
		if (isClosed == 0)
			break;
		++it;
	}
	if (isClosed != 0)
		throw (Error::ErrorBracketParseFile());
	addServer(newServer);
}

void	Context::configFileParser(const std::string& fileName) {
	FtString					content;
	std::vector<std::string>	tokens;


	if (getContent(fileName, content))
		throw (Error::CanNotOpenFile(fileName));
	addSpace(content, ';');
	tokens = content.ft_split(" \n\b\t\r\v\f");
	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		if (*it == "server")
			parseAndAddServer(++it, tokens.end());
	}
}