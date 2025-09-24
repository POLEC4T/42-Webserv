/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:07:38 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/24 23:25:42 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <string>
# include "Location.hpp"

class	Server {
	private:
		std::string						_name;
		int								_port;
		int								_clientMaxBodySize;
		std::map<std::string, Location>	_locations;
	
	public:
		//Canonical constructor
		Server();
		Server(const Server& cpy);
		Server& operator=(const Server& other);
		~Server();

		//Constructor with affectation values
		Server(std::string, int, int);
		
		//Setter
		void	setName(std::string);
		void	setPort(int);
		void	setClientMaxBodySize(int);
		void	setLocation(Location&);
		
		//Getter
		std::string						getName();
		int								getPort();
		int								getClientMaxBodySize();
		std::map<std::string, Location>	getLocationMap();
		
		//Specific map
		void		pushLocation(Location&);
		Location&	getLocationByName(std::string);
		
};

#endif