/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:16:33 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/30 11:10:56 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

# include "Server.hpp"
# include "FtString.hpp"
# include "Location.hpp"

class Location;

class Context {
	private:
		std::vector<Server>			_servers;
		Server						_currentServer;
		Location					_currentLocation;
		std::map<int, ErrorPage>	_mapDefaultErrorPage;
		int							_epollfd;

	public:
		Context();
		Context(const Context& cpy);
		Context& operator=(const Context& other);
		~Context();
		
		//Getter
		std::vector<Server>&			getServers() ;
		const std::map<int, ErrorPage>&	getMapDefaultErrorPage() const ;
		
		//Setter
		void	addServer(const Server& server);
		void	setEpollFd(int fd);
		
		//functions
		void	configFileParser(const std::string& fileName, std::map<int, ErrorPage>);
		void	parseAndAddServer(std::vector<std::string>::iterator&,
				const std::vector<std::string>::iterator&, std::map<int, ErrorPage>);
		void	parseAndSetMapDefaultErrorPage();
		bool	isListenerFd(int fd) const;
		Server&	getRelatedServer(int fd);
};

#endif