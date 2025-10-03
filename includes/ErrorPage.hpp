/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 00:02:01 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/03 23:20:09 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORPAGE_HPP
# define ERRORPAGE_HPP

# include "APage.hpp"

//class ErrorPage inherit from APage got the private attribute _name, _content, _root, with the associated functions
class ErrorPage : public APage {
	public:
		//Canonical form
		ErrorPage();
		ErrorPage(const ErrorPage& cpy);
		ErrorPage& operator=(const ErrorPage& other);
		~ErrorPage();

		//constructor with assignation values
		ErrorPage(std::string name, std::string root);
		ErrorPage(std::string name, std::string root, std::string content, int code);
};

#endif