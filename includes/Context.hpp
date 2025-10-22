/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:16:33 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/22 17:01:38 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
# define CONTEXT_HPP

# include "Server.hpp"
# include "FtString.hpp"

class Context {
	private:
		std::vector<Server>				_servers;
		Server							_currentServer;
		Location						_currentLocation;
		std::map<int, ErrorPage>		_mapDefaultErrorPage;

	public:
		Context();
		Context(const Context& cpy);
		Context& operator=(const Context& other);
		~Context();
		
		//Getter
		const std::vector<Server>		getServers() const ;
		const std::map<int, ErrorPage>&	getMapDefaultErrorPage() const ;
		
		//Setter
		void	addServer(const Server& server);
		
		//functions
		void	configFileParser(const std::string& fileName, std::map<int, ErrorPage>);
		void	parseAndAddServer(std::vector<std::string>::iterator&,
				const std::vector<std::string>::iterator&, std::map<int, ErrorPage>);
		void	parseAndSetMapDefaultErrorPage();
};

#endif