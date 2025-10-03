/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 23:14:32 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/03 23:47:02 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser() {}

ConfigFileParser::~ConfigFileParser() {}

const char* ConfigFileParser::CanNotOpenFile::what() const throw() {
	return "Can't open the file.";
}

std::vector<std::string>	ft_split(std::string fileContent, const std::string delimiter) {
	
}

void	ConfigFileParser::configFileParser(std::string fileName) {
	std::string		content;
	std::ifstream	file(fileName, std::ios::in);

	if (!file.is_open())
		throw (CanNotOpenFile());
}