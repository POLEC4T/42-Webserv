/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:07:38 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/03 23:20:30 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Location.hpp"
# include "ErrorPage.hpp"

class	Server {
	private:
		std::vector<std::string>		_name;
		int								_port;
		int								_clientMaxBodySize;
		std::map<std::string, Location>	_mapLocation;
		std::map<int, ErrorPage>		_mapErrorPage;
	
	public:
		//Canonical constructor
		Server();
		Server(const Server& cpy);
		Server& operator=(const Server& other);
		~Server();

		//Constructor with affectation values
		Server(int, int);
		
		//Setter
		void	addName(std::string);
		void	setPort(int);
		void	setClientMaxBodySize(int);
		
		//Getter
		std::vector<std::string>		getName();
		int								getPort();
		int								getClientMaxBodySize();
		
		//Specific map
		void		pushLocation(const Location&);
		APage&		getLocationByName(const std::string&);
		void		pushErrorPage(const ErrorPage&);
		APage&		getErrorPageByCode(const int);

		//exception class if page and error page not found
		class NoPageFound: public std::exception {
			public:
				const char* what() const throw() ;
		};

};

#endif