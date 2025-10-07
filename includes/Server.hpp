/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:07:38 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/07 11:20:28 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Location.hpp"
# include "ErrorPage.hpp"

class	Server {
	private:
		std::vector<std::string>		_name;
		std::string						_host;
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
		void	setHost(const std::string& host);
		void	setClientMaxBodySize(std::string);
		void	setPort(std::string);
		
		//Getter
		std::vector<std::string>		getNames();
		int								getPort();
		int								getClientMaxBodySize();
		const std::string&					getHost() const;
		
		//Specific map
		void		pushLocation(const Location&);
		APage&		getLocationByName(const std::string&);
		void		pushErrorPage(const ErrorPage&);
		APage&		getErrorPageByCode(const int);
		void		addErrorPage(std::string& code, std::string& root);

		//exception class if page and error page not found
		class NoPageFound: public std::exception {
			public:
				const char* what() const throw() ;
		};


		Location	configFileLocationParser(std::vector<std::string>::iterator&);
};

#endif