/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPages.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 00:02:01 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/25 00:04:56 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORPAGES_HPP
# define ERRORPAGES_HPP

# include "APage.hpp"
# include <map>
# include <string>

class ErrorPages : public APage {
	private:
		std::map<int, std::string>	_rootErrorPageCode;
		
	public:
		//Canonical form
		ErrorPages();
		ErrorPages(const ErrorPages& cpy);
		ErrorPages& operator=(const ErrorPages& other);
		~ErrorPages();
};

#endif