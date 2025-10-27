/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dorianmazari <dorianmazari@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 12:35:49 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/20 13:41:25 by dorianmazar      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorPage.hpp"

ErrorPage::ErrorPage() : APage() {}

ErrorPage::ErrorPage(const ErrorPage &cpy) : APage(cpy) {}

ErrorPage &ErrorPage::operator=(const ErrorPage &other) {
  if (this != &other) {
    this->_name = other._name;
    this->_root = other._root;
    this->_content = other._content;
    this->_code = other._code;
  }
  return *this;
}

ErrorPage::~ErrorPage() {}

// constructor with assignement values
ErrorPage::ErrorPage(std::string name, std::string root) : APage(name, root) {}

ErrorPage::ErrorPage(std::string name, std::string root, std::string content,
                     int code)
    : APage(name, root, content, code) {}

ErrorPage::ErrorPage(const std::string name, const std::string content,
                     int code)
    : APage(name, content, code) {}
