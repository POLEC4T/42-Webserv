/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   APage.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:56:14 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/25 09:45:16 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APAGE_HPP
# define APAGE_HPP

# include <string>

class APage {
	protected:
		std::string	_name;
		std::string	_root;
		std::string	_content;

	public:
		//Canonic
		APage();
		APage(const APage& cpy);
		APage& operator=(const APage& other);
		virtual ~APage() = 0;

		//constructor with assignation values
		APage(std::string name, std::string root, std::string content);

		//Setter
		void	setName();
		void	setRoot();
		void	setContent();

		//Getter
		std::string	getName();
		std::string	getRoot();
		std::string	getContent();
};

#endif