/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 23:12:22 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/06 14:02:05 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILEPARSER_HPP
# define CONFIGFILEPARSER_HPP

# include "Headers.h"

class ConfigFileParser {
	public:
		ConfigFileParser();
		~ConfigFileParser();

		void	configFileParser(const std::string& fileName);

		//exception class if can't open a file
		class CanNotOpenFile: public std::exception {
			public:
				const char* what() const throw() ;
		};
};

#endif