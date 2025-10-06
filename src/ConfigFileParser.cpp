/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 23:14:32 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/06 23:46:01 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser() {}

ConfigFileParser::~ConfigFileParser() {}

const char* ConfigFileParser::CanNotOpenFile::what() const throw() {
	return "Can't open the file.";
}

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

bool	isBracketClosed(const std::string& content) {
	int	brackets = 0;
	for (size_t i = 0; i < content.length(); i++)
	{
		if (content[i] == '{')
			brackets++;
		else if (content[i] == '}')
			brackets--;
	}

	return (brackets == 0);
}

int	countServer(const std::string& content) {
	int	i = 0;
	size_t	res = -1;

	while ((res = content.find("server ", res + 1)) !=
				std::string::npos){
		i++;
	}
	return i;
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
				content += line;
		}
	}
	file.close();
	return 0;
}

// void	ConfigFileParser::configFileParser(const std::string& fileName) {
// 	std::string					content;
// 	std::vector<std::string>	test;


// 	if (getContent(fileName, content))
// 		throw (CanNotOpenFile());
// 	test = ft_split(content, " \n\b\t\r\v\f");
// 	for (std::vector<std::string>::iterator it = test.begin(); it != test.end(); it++) {
// 		if (*it == "server")
// 	}
	
// }