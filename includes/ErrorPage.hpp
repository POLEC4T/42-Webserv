/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 00:02:01 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/13 18:03:25 by dmazari          ###   ########.fr       */
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
		ErrorPage(std::string name, std::string content, int code);
};

#endif