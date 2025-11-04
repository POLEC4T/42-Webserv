/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:07:38 by mazakov           #+#    #+#             */
/*   Updated: 2025/11/03 16:42:42 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

# include "Location.hpp"
# include "ErrorPage.hpp"
# include "Client.hpp"
# include <unistd.h>
# include <algorithm>


class	Server {
	private:
		std::vector<std::string>		_name;
		std::string						_host;
		std::vector<std::string>		_ports;
		long long						_clientMaxBodySize;
		std::map<std::string, Location>	_mapLocation;
		std::map<int, ErrorPage>		_mapErrorPage;
		std::map<int, ErrorPage>		_mapDefaultErrorPage;
		std::map<int, Client>			_mapClients;
		int								_timedOut;
		std::vector<int>				_sockfds;
	
	public:
		//Canonical constructor
		Server();
		Server(const Server& cpy);
		Server& operator=(const Server& other);
		~Server();

		//Constructor with affectation values
		Server(std::map<int, ErrorPage>);
		
		//Setter
		void	addName(const std::string&);
		void	setClientMaxBodySize(int);
		void	setClientMaxBodySize(std::string);
		void	setHost(const std::string&);
		void	addPort(const std::string&);
		void	setTimeOut(const std::string);
		
		//Getter
		const std::vector<std::string>&		getNames() const ;
		const std::vector<std::string>&		getPorts() const;
		long long							getClientMaxBodySize() const ;
		int									getTimedOutValue() const ;
		const std::string&					getHost() const;
		std::map<int, Client>&				getClients();
		
		//Specific map
		APage&					getLocationByName(const std::string&);
		ErrorPage&				getErrorPageByCode(const int);
		void					setDefaultMapErrorPage(const std::map<int, ErrorPage>&);

		void					addLocation(const Location &);
		void					addErrorPage(ErrorPage &);
		void					addErrorPage(const std::string &code, const std::string &root);
		Client&					getClient(int fd);
		void					addClient(const Client &);
		void					deleteAllClients();
		void					deleteClient(int fd);
		void					addSockfd(int fd);
		const std::vector<int>&	getSockfds() const;
		bool					isClient(int fd) const;
		bool					isListener(int fd) const;

	std::map<std::string, Location> &getLocations();

	// Parser
	void parseAndAddLocation(std::vector<std::string>::iterator &,
							const std::vector<std::string>::iterator);
};

#endif