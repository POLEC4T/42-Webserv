/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 00:02:01 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/25 09:27:37 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORPAGE_HPP
# define ERRORPAGE_HPP

# include "APage.hpp"
# include <map>
# include <string>

class ErrorPage : public APage {
	private:
		std::string	_name;
		std::string	_root;
		std::string	_code;
		std::string	_content;
		
	public:
		//Canonical form
		ErrorPage();
		ErrorPage(const ErrorPage& cpy);
		ErrorPage& operator=(const ErrorPage& other);
		~ErrorPage();
};

#endif