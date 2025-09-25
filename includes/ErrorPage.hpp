/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 00:02:01 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/25 09:46:00 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORPAGE_HPP
# define ERRORPAGE_HPP

# include "APage.hpp"
# include <map>

//class ErrorPage inherit from APage got the private attribute _name, _content, _root, with the associated functions
class ErrorPage : public APage {
	private:
		int			_code;
		
	public:
		//Canonical form
		ErrorPage();
		ErrorPage(const ErrorPage& cpy);
		ErrorPage& operator=(const ErrorPage& other);
		~ErrorPage();

		//constructor with assignation values
		ErrorPage(std::string name, std::string root, std::string content, int code);

		//setter
		void	setCode();
		
		//Getter
		int	getCode();
};

#endif