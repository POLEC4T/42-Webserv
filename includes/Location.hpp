/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:10:13 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/06 13:20:02 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "AHttpMethod.hpp"
# include "APage.hpp"

//class Location inherit from APage got the private attribute _name, _content, _root, with the associated functions
class	Location : public APage {
	private:
		std::vector<AHttpMethod*>	_allowedMethods;
		std::vector<std::string>	_index;
		bool						_autoIndex;
		size_t						_clientMaxBodySize;
		std::string					_return;
		std::string					_cgiExtension;
		std::string					_cgiPath;


	public:
		//Canonical constructor
		Location();
		Location(const Location& cpy);
		Location& operator=(const Location& other);
		~Location();

		//Constructor with values affectation, only root and name other can rest empty
		Location(std::string name, std::string root);
		Location(std::string name, std::string root, std::string content, int code);

		//Setter
		void	setAllowedMethods(const std::vector<AHttpMethod*>&);
		void	setAutoIndex(const bool);
		void	setCgiExtension(const std::string&);
		void	setCgiPath(const std::string&);
		void	setClientMaxBodySize(size_t);

		//Getter
		std::vector<AHttpMethod*>	getAllowedMethods();
		std::vector<std::string>	getIndex();
		bool						getAutoIndex();
		std::string					getCgiExtension();
		std::string					getCgiPath();
		size_t						getClientMaxBodySize();

		//vector functions
		void	addIndex(const std::string&);
		void	pushMethod(AHttpMethod* method);
};

#endif