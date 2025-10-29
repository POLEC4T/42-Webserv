/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FtString.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 14:38:09 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/29 17:34:07 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FtString.hpp"

FtString::FtString() : std::string() {}

FtString::FtString(const std::string &copy) : std::string(copy) {}

FtString::FtString(const FtString &copy) : std::string(copy) {}

FtString &FtString::operator=(const FtString &other) {
  if (this != &other) {
    std::string::operator=(other);
  }
  return (*this);
}

FtString &FtString::operator=(const std::string &other) {
  if (this != &other) {
    std::string::operator=(other);
  }
  return (*this);
}

FtString::~FtString() {}

#include <iostream>

/**
 * @param delimiters a string containing all the delimiter characters
 */
std::vector<std::string>
FtString::ft_split(const std::string &delimiters) const {
  std::vector<std::string> tokens;
  std::string token;
  size_t j;
  size_t i = 0;

  while (i < this->size()) {
    while (i < this->size() && delimiters.find(at(i)) != std::string::npos)
      i++;
    j = i;
    while (i < this->size() && delimiters.find(at(i)) == std::string::npos)
      i++;
    if (j < i) {
      token = this->substr(j, i - j);
      tokens.push_back(token);
    }
  }
  return (tokens);
}

/**
 * @param delimiter a string containing the delimiter, considered as a whole
 */
std::vector<std::string>
FtString::ft_split_word(const std::string &delimiter) const {
  std::vector<std::string> tokens;
  std::string token;
  size_t j;
  size_t i = 0;
  bool isDelim;

  while (i < this->size()) {
    isDelim = this->substr(i, delimiter.size()) == delimiter;
    if (isDelim) {
      i += delimiter.size();
      continue;
    }
    j = i + 1;
    while (++j < this->size() && !isDelim)
      isDelim = this->substr(j, delimiter.size()) == delimiter;
    token = this->substr(i, (j - 1) - i);
    tokens.push_back(token);
    i = j + delimiter.size() - 1;
    i++;
  }
  return (tokens);
}

/**
 * @return true if the string ends with one of the characters in str
 */
bool FtString::endsWith(const std::string &str) const {
  if (this->empty() || str.empty())
    return (false);
  if (str.find(at(this->size() - 1)) != std::string::npos)
    return (true);
  return (false);
}

/**
 * @return true if the string starts or ends with one of the characters in str
 */
bool FtString::startsOrEndsWith(const std::string &str) const {
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

/**
 * @returns true if string has @param nbElems separated by the @param delimiter
 */
bool FtString::hasXElemsSepByDel(int nbElems,
                                 const std::string &delimiter) const {
  if (nbElems <= 0)
    return false;
  std::vector<std::string> elems = this->ft_split(delimiter);
  return (elems.size() == static_cast<size_t>(nbElems));
}