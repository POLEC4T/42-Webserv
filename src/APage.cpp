/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   APage.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 12:22:06 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/25 13:34:52 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "APage.hpp"

//Canonic
APage::APage(): _code(0) {}

APage::APage(const APage& cpy) {
	_name = cpy._name;
	_root = cpy._root;
	_content = cpy._content;
	_code = cpy._code;
}

APage& APage::operator=(const APage& other) {
	if (this != &other)
	{
		this->_name = other._name;
		this->_root = other._root;
		this->_content = other._content;
		this->_code = other._code;
	}
	return (*this);
}

APage::~APage() {}

//constructor with assignement value
APage::APage(std::string name, std::string root) {
	_name = name;
	_root = root;
}

APage::APage(std::string name, std::string root, std::string content, int code) {
	_name = name;
	_root = root;
	_content = content;
	_code = code;
}


//Setter
void	APage::setName(std::string name) {
	_name = name;
}

void	APage::setRoot(std::string root) {
	_root = root;
}

void	APage::setContent(std::string content) {
	_content = content;
}

void	APage::setCode(int code) {
	_code = code;
}

//Getter
std::string	APage::getName() const {
	return _name;
}

std::string	APage::getRoot() const {
	return _root;
}

std::string	APage::getContent() const {
	return _content;
}

int	APage::getCode() const {
	return _code;
}
