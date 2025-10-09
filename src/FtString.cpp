/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FtString.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 14:38:09 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/09 20:09:25 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FtString.hpp"

FtString::FtString() : std::string() {}

FtString::FtString(const std::string& copy) : std::string(copy) {}

FtString::FtString(const FtString& copy) : std::string(copy) {}

FtString& FtString::operator=(const FtString& other) {
	if (this != &other) {
		std::string::operator=(other);
	}
	return (*this);
}

FtString& FtString::operator=(const std::string& other) {
	if (this != &other) {
		std::string::operator=(other);
	}
	return (*this);
}

FtString::~FtString() {}

std::vector<std::string> FtString::ft_split(const std::string& delimiters) const {
	std::vector<std::string>	tokens;
	std::string					token;
	size_t						j;
	size_t						i;
	bool isNotDelim;

	for (i = 0; i < this->size(); i++) {
		isNotDelim = delimiters.find(at(i)) == std::string::npos;
		if (isNotDelim) {
			for (j = i + 1; j < this->size() && isNotDelim; j++) {}
			token = this->substr(i, j - i);
			tokens.push_back(token);
			i = j;
		}
	}
	return (tokens);
}

/**
 * @return true if the string ends with one of the characters in str
 */
bool FtString::endsWith(const std::string& str) const {
	if (this->empty() || str.empty())
		return (false);
	if (str.find(at(this->size() - 1)) != std::string::npos)
		return (true);
	return (false);
}


/**
 * @return true if the string starts or ends with one of the characters in str
 */
bool FtString::startsOrEndsWith(const std::string& str) const {
	if (this->empty() || str.empty())
		return false;
	if (str.find(at(0)) != std::string::npos)
		return true;
	if (str.find(at(this->size() - 1)) != std::string::npos)
		return true;
	return false;
}

/**
 * @brief erases whitespaces at the left of the string
 */
void FtString::ltrim() {
	iterator it = this->begin();
	iterator ite = this->end();
	while (it < ite && std::isspace(*it))
		it++;
	this->erase(this->begin(), it);
}

/**
 * @brief erases whitespaces at the right of the string
 */
void FtString::rtrim() {
	reverse_iterator it = this->rbegin();
	reverse_iterator ite = this->rend();
	while (it < ite && std::isspace(*it))
		it++;
	this->erase(it.base(), this->end());
}

/**
 * @brief erases whitespaces at the right and left of the string
 */
void FtString::trim() {
	this->ltrim();
	this->rtrim();
}

