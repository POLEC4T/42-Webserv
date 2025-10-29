/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   APage.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:56:14 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/29 10:38:25 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APAGE_HPP
# define APAGE_HPP


#include <string>
#include <sstream>

class Location;

class APage {
	protected:
		std::string	_name;
		std::string	_root;
		std::string	_content;
		int			_code;

	public:
		//Canonic
		APage();
		APage(const APage& cpy);
		APage& operator=(const APage& other);
		virtual ~APage() = 0;

		//constructor with assignation values
		APage(std::string name, std::string root);
		APage(std::string name, std::string root, std::string content, int code);
		APage(std::string name, std::string content, int code);

		//Setter
		void	setName(std::string);
		void	setRoot(std::string);
		void	setContent(std::string);
		void	setCode(int code);
		void	setCode(std::string);

		//Getter
		virtual const std::string&	getName() const;
		virtual const std::string&	getRoot() const;
		virtual const std::string&	getContent() const;
		virtual int					getCode() const;
};

#endif