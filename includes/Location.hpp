/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:10:13 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/25 09:26:30 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <string>
# include <vector>
# include "AHttpMethod.hpp"
# include "APage.hpp"

class	Location : public APage {
	private:
		std::string					_name;
		std::string					_root;
		std::vector<AHttpMethod>	_allowedMethods;
		bool						_autoIndex;
		std::string					_index;
		std::string					_cgiExtension;
		std::string					_cgiPath;

		std::string					_content;

	public:
		//Canonical constructor
		Location();
		Location(const Location& cpy);
		Location& operator=(const Location& other);
		~Location();

		//Constructor with values affectation, only root and name other can rest empty
		Location(std::string _name, std::string _root);

		//Setter
		void	setName(std::string);
		void	setRoot(std::string);
		void	setAllowedMethods(std::vector<AHttpMethod>);
		void	setAutoIndex(bool);
		void	setIndex(std::string);
		void	setCgiExtension(std::string);
		void	setCgiPath(std::string);

		//Getter
		std::string					getName();
		std::string					getRoot();
		std::vector<AHttpMethod>	getAllowedMethods();
		bool						getAutoIndex();
		std::string					getIndex();
		std::string					getCgiExtension();
		std::string					getCgiPath();

		//Functions for vector<Methods>
		void	pushMethod(AHttpMethod& method);
};

#endif