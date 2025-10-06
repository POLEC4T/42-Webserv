/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:16:33 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/06 23:50:17 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
# define CONTEXT_HPP

# include "Server.hpp"
# include "ConfigFileParser.hpp"

class Context {
	private:
		std::vector<Server>	_servers;
		Server				_actualServer;
		Location			_actualLocation;

	public:
		Context();
		Context(const Context& cpy);
		Context& operator=(const Context& other);
		~Context();

		//Getter
		std::vector<Server>	getServers();

		//Setter
		void	addServer(const Server& server);

		//functions
		void	configFileParser(const std::string& fileName);
		Server	configFileServerParser(std::vector<std::string>::iterator&,
				const std::vector<std::string>::iterator&);

		//exceptions
		class CanNotOpenFile: public std::exception {
			public:
				const char* what() const throw() ;
		};

		class ErrorBracketParseFile: public std::exception {
			public:
				const char* what() const throw() ;
		};
};

#endif