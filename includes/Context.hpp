/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:16:33 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/03 23:17:23 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
# define CONTEXT_HPP

# include "Server.hpp"
# include "ConfigFileParser.hpp"

class Context {
	private:
		ConfigFileParser	_configFileParser;
		std::vector<Server>	_servers;

	public:
		Context();
		Context(const Context& cpy);
		Context& operator=(const Context& other);
		~Context();

		//Getter
		std::vector<Server>	getServers();

		//Setter
		void	addServer(const Server& server);
};

#endif