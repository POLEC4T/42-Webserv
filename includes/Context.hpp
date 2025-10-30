/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:16:33 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/30 14:48:44 by mniemaz          ###   ########.fr       */
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
		~Context();
		
		//Getter
		std::vector<Server>&			getServers() ;
		const std::map<int, ErrorPage>&	getMapDefaultErrorPage() const ;
		int								getEpollFd() const ;
		
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