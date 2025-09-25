/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:10:13 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/25 12:58:33 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <vector>
# include "AHttpMethod.hpp"
# include "APage.hpp"

//class Location inherit from APage got the private attribute _name, _content, _root, with the associated functions
class	Location : public APage {
	private:
		std::vector<AHttpMethod>	_allowedMethods;
		bool						_autoIndex;
		std::string					_index;
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
		void	setAllowedMethods(const std::vector<AHttpMethod>&);
		void	setAutoIndex(const bool);
		void	setIndex(const std::string&);
		void	setCgiExtension(const std::string&);
		void	setCgiPath(const std::string&);

		//Getter
		std::vector<AHttpMethod>	getAllowedMethods();
		bool						getAutoIndex();
		std::string					getIndex();
		std::string					getCgiExtension();
		std::string					getCgiPath();

		//Functions for vector<Methods>
		void	pushMethod(const AHttpMethod& method);
};

#endif