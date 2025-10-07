/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:19:40 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/07 11:52:31 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Context.hpp"

# define BRACKET_OPENED 0
# define BRACKET_CLOSED 1

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
std::vector<Server>	Context::getServers() {
	return _servers;
}

//Setter
void	Context::addServer(const Server& server) {
	_servers.push_back(server);
}


//Exception
const char* Context::CanNotOpenFile::what() const throw() {
	return "Can't open the file.";
}

const char* Context::ErrorBracketParseFile::what() const throw() {
	return "Not all brackets are closed or previously opened.";
}

//functions

bool	isDelimiter(char c, const std::string& delimiters) {
	for (size_t i = 0; i < delimiters.length(); i++)
	{
		if (delimiters[i] == c)
			return true;
	}
	return false;
}

std::vector<std::string>	ft_split(const std::string& s, const std::string& delimiters) {
	std::vector<std::string>	tokens;
	std::string					token;
	size_t						j;
	size_t						i;

	for (i = 0; i < s.length(); i++) {
		if (!isDelimiter(s[i], delimiters)) {
			for (j = i + 1; j < s.length() && !isDelimiter(s[j], delimiters); j++) {}
			token = s.substr(i, j - i);
			tokens.push_back(token);
			i = j;
		}
	}
	return (tokens);
}

int	getContent(std::string fileName, std::string& content) {
	std::string		line;
	size_t			i;
	std::ifstream	file(fileName.c_str(), std::ios_base::in);

	if (!file.is_open())
		return 1;
	while (getline(file, line)) {
		if (!line.empty())
		{
			for (i = 0; i < line.length() && iswspace(line[i]); i++) {}
			if (line[i] != '#')
			{
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

void	Context::configFileServerParser(std::vector<std::string>::iterator& it,
		const std::vector<std::string>::iterator& itEnd) {
	int		brackets[2] = {0, 0};
	Server	newServer;

	while (it != itEnd)
	{
		if (*it == "{")
			brackets[BRACKET_OPENED]++;
		else if (*it == "}")
			brackets[BRACKET_CLOSED]++;
		if (brackets[BRACKET_OPENED] == brackets[BRACKET_CLOSED])
			break;
		if (*it == "host") {
			++it;
			if (it != itEnd) {
				newServer.setHost(*it);
			}
		}
		else if (*it == "port") {
			++it;
			if (it != itEnd)
				newServer.setPort(*it);
		}
		else if (*it == "client_max_body_size") {
			++it;
			if (it != itEnd)
				newServer.setClientMaxBodySize(*it);
		}
		else if (*it == "server_name") {
			++it;
			while (it != itEnd && *it != ";") {
				newServer.addName(*it);
				++it;
			}
		}
		else if (*it == "error_page") {
			if ((it + 1) != itEnd && (it + 2) != itEnd) {
				newServer.addErrorPage(*(it + 1), *(it + 2));
				it += 2;
			}
		}
		else if (*it == "location") {
			++it;
			if (it != itEnd)
				newServer.configFileLocationParser(it, itEnd);
		}
		++it;
	}
	if (brackets[BRACKET_OPENED] != brackets[BRACKET_CLOSED])
		throw (ErrorBracketParseFile());
	addServer(newServer);
}

void	Context::configFileParser(const std::string& fileName) {
	std::string					content;
	std::vector<std::string>	tokens;


	if (getContent(fileName, content))
		throw (CanNotOpenFile());
	addSpace(content, ';');
	tokens = ft_split(content, " \n\b\t\r\v\f");
	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		std::cout << *it << std::endl;
		if (*it == "server")
			configFileServerParser(++it, tokens.end());
	}





	for (size_t i = 0; i < _servers.size(); ++i) {
		std::cout << "Server " << i + 1 << ":\n";
		std::cout << "  Host: " << _servers[i].getHost() << "\n";
		std::cout << "  Port: " << _servers[i].getPort() << "\n";
		std::cout << "  Client Max Body Size: " << _servers[i].getClientMaxBodySize() << "\n";
		std::cout << "  Names: ";
		const std::vector<std::string>& names = _servers[i].getNames();
		for (size_t j = 0; j < names.size(); ++j) {
			std::cout << names[j];
			if (j + 1 < names.size()) std::cout << ", ";
		}
		std::cout << "\n";
		
		// Print locations
		std::map<std::string, Location>& locations = _servers[i].getLocations();
		if (!locations.empty()) {
			std::cout << "  Locations:\n";
			for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); ++it) {
				std::cout << "    Location: " << it->first << "\n";
				std::cout << "      Root: " << it->second.getRoot() << "\n";
				std::cout << "      Auto Index: " << (it->second.getAutoIndex() ? "on" : "off") << "\n";
				std::cout << "      Client Max Body Size: " << it->second.getClientMaxBodySize() << "\n";
				
				// Print index files
				const std::vector<std::string>& indices = it->second.getIndex();
				if (!indices.empty()) {
					std::cout << "      Index: ";
					for (size_t k = 0; k < indices.size(); ++k) {
						std::cout << indices[k];
						if (k + 1 < indices.size()) std::cout << ", ";
					}
					std::cout << "\n";
				}
				
				// Print allowed methods (you'll need a getter for this)
				// std::cout << "      Allowed Methods: ...\n";
				
				if (!it->second.getUploadPath().empty())
					std::cout << "      Upload Path: " << it->second.getUploadPath() << "\n";
				if (!it->second.getReturn().empty())
					std::cout << "      Return: " << it->second.getReturn() << "\n";
				if (!it->second.getCgiExtension().empty())
					std::cout << "      CGI Extension: " << it->second.getCgiExtension() << "\n";
				if (!it->second.getCgiPath().empty())
					std::cout << "      CGI Path: " << it->second.getCgiPath() << "\n";
			}
		}
		std::cout << "\n";
}
}