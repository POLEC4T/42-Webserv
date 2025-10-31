/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:16:33 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/31 12:48:24 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

# include "Server.hpp"
# include "FtString.hpp"
# include "Location.hpp"

class Location;

struct s_Cgi {
	int pid;
	int outputFd;
	int clientFd;
	int startTime;
	int timeOut;
	std::string output;
	bool done;
}	t_Cgi;

class Context {
	private:
		std::vector<Server>			_servers;
		Server						_currentServer;
		Location					_currentLocation;
		std::map<int, ErrorPage>	_mapDefaultErrorPage;
		int							_epollfd;
		std::map<int, t_Cgi>		_mapRunningCgi;

	public:
		Context();
		~Context();
		
		//Getter
		bool							isRunningCgi(int fd);
		t_Cgi							getRunningCgi(int fd);
		std::vector<Server>&			getServers() ;
		const std::map<int, ErrorPage>&	getMapDefaultErrorPage() const ;
		int								getEpollFd() const ;
		
		//Setter
		void	addCgi(t_Cgi);
		void	addServer(const Server& server);
		void	setEpollFd(int fd);
		
		//functions
		void	checkTimedOutCgi();
		void	configFileParser(const std::string& fileName, std::map<int, ErrorPage>);
		void	parseAndAddServer(std::vector<std::string>::iterator&,
				const std::vector<std::string>::iterator&, std::map<int, ErrorPage>);
		void	parseAndSetMapDefaultErrorPage();
		bool	isListenerFd(int fd) const;
		Server&	getRelatedServer(int fd);
};

#endif