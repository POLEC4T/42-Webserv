/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:16:33 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/07 14:38:23 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
# define CONTEXT_HPP

# include "Server.hpp"

class Context {
	private:
		std::vector<Server>	_servers;
		Server				_currentServer;
		Location			_currentLocation;

	public:
		Context();
		Context(const Context& cpy);
		Context& operator=(const Context& other);
		~Context();

		//Getter
		const std::vector<Server>	getServers() const ;

		//Setter
		void	addServer(const Server& server);

		//functions
		void	configFileParser(const std::string& fileName);
		void	parseAndAddServer(std::vector<std::string>::iterator&,
					const std::vector<std::string>::iterator&);
};

#endif