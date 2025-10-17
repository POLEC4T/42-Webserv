/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:07:38 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/17 13:04:11 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Location.hpp"
# include "ErrorPage.hpp"
# include "Client.hpp"
# include <unistd.h>

class	Server {
	private:
		std::vector<std::string>		_name;
		std::string						_host;
		std::string						_port;
		int								_clientMaxBodySize;
		std::map<std::string, Location>	_mapLocation;
		std::map<int, ErrorPage>		_mapErrorPage;
		std::map<int, ErrorPage>		_mapDefaultErrorPage;
		std::map<int, Client>			_mapClients;
	
	public:
		//Canonical constructor
		Server();
		Server(const Server& cpy);
		Server& operator=(const Server& other);
		~Server();

		//Constructor with affectation values
		Server(int, int);
		Server(std::map<int, ErrorPage>);
		
		//Setter
		void	addName(const std::string&);
		void	setClientMaxBodySize(int);
		void	setClientMaxBodySize(std::string);
		void	setHost(const std::string&);
		void	setPort(const std::string&);
		
		//Getter
		const std::vector<std::string>&		getNames() const ;
		const std::string&					getPort() const;
		int									getClientMaxBodySize() const ;
		const std::string&					getHost() const;
		
		//Specific map
		APage&		getLocationByName(const std::string&);
		ErrorPage&		getErrorPageByCode(const int);
		void			setDefaultMapErrorPage(const std::map<int, ErrorPage>&);

		void		addLocation(const Location&);
		void		addErrorPage(const ErrorPage&);
		void		addErrorPage(const std::string& code, const std::string& root);
		Client&		getClient(int fd);
		void		addClient(const Client&);
		void		removeClient(int fd);

		std::map<std::string, Location>&	getLocations() ;

		//Parser
		void		parseAndAddLocation(std::vector<std::string>::iterator&, 
					const std::vector<std::string>::iterator);
};

#endif