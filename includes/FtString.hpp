/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FtString.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dorianmazari <dorianmazari@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 14:33:39 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/20 13:42:18 by dorianmazar      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FTSTRING_HPP
#define FTSTRING_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * 	@warning NEVER do a new FtString, else, it will leak because std::string
 *  does not have a virtual destructor
 */
class FtString : public std::string {
public:
  FtString();
  FtString(const std::string &copy);
  FtString(const FtString &copy);
  FtString &operator=(const FtString &other);
  FtString &operator=(const std::string &other);

  typedef std::string::iterator iterator;
  typedef std::string::const_iterator const_iterator;
  typedef std::string::reverse_iterator reverse_iterator;
  typedef std::string::const_reverse_iterator const_reverse_iterator;

  ~FtString();

  std::vector<std::string> ft_split(const std::string &delimiters) const;
  std::vector<std::string> ft_split_word(const std::string &delimiter) const;
  bool endsWith(const std::string &str) const;
  bool startsOrEndsWith(const std::string &str) const;
  bool hasXElemsSepByDel(int nbElems, const std::string &delimiter) const;

  template <typename T> static std::string my_to_string(T value) {
    std::ostringstream oss;
    oss << value;
    return (oss.str());
  }

  // modifiers
  void ltrim();
  void rtrim();
  void trim();
};

#endif