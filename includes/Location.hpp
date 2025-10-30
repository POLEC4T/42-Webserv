/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:10:13 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/30 11:52:34 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "APage.hpp"
#include <fstream>
#include <vector>
#include <map>

/**
 * class Location inherit from APage got the private attribute _name, _content,
 * _root, with the associated functions
 */
class Location : public APage {
  private:
	// std::vector<AHttpMethod*>	_allowedMethods;
	std::vector<std::string> _allowedMethods;
	std::vector<std::string> _index;
	bool _autoIndex;
	long long _clientMaxBodySize;
	std::string _uploadPath;
	std::string _return;
	std::map<std::string, std::string> _cgi;

  public:
	// Canonical constructor
	Location();
	Location(const Location &cpy);
	Location &operator=(const Location &other);
	~Location();

	// Constructor with values affectation, only root and name other can rest
	// empty
	Location(std::string name, std::string root);
	Location(std::string name, std::string root, std::string content, int code);

	// Setter
	//  void	setAllowedMethods(const std::vector<AHttpMethod*>&);
	void setAutoIndex(const bool);
	void addCgi(const std::string &extension, const std::string &path);
	void setClientMaxBodySize(size_t);
	void setClientMaxBodySize(std::string);
	void setReturn(const std::string &);
	void setUploadPath(const std::string &);

	// Getter
	//  std::vector<AHttpMethod*>	getAllowedMethods();
	const std::vector<std::string> getIndex();

	bool getAutoIndex();
	long long getClientMaxBodySize() const;

	const std::string &getCgiByExtension(const std::string &extension) const;
	const std::string &getUploadPath();
	const std::string &getReturn();

	const std::vector<std::string> &getAllowedMethods() const;

	// vector functions
	void addAllowedMethods(const std::string &);
	void addIndex(const std::string &);

	// void	pushMethod(AHttpMethod* method);
};

#endif