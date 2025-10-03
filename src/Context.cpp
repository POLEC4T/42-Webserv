/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 15:19:40 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/03 15:27:47 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Context.hpp"

Context::Context() {}

Context::~Context() {}

Context::Context(const Context& cpy) {
	std::vector<Server>::const_iterator it = cpy._servers.begin();

	while (it != cpy._servers.end())
	{
		_servers.push_back(*it);
		it++;
	}
}

Context&	Context::operator=(const Context& other) {
	if (this != &other)
	{
		this->_servers.erase(_servers.begin(), _servers.end());
		std::vector<Server>::const_iterator it = other._servers.begin();
		while (it != other._servers.end())
		{
			_servers.push_back(*it);
			it++;
		}
	}
	return *this;
}


//Getter
std::vector<Server>	Context::getServers() {
	return _servers;
}



//Setter
void	Context::addServer(const Server& server) {
	_servers.push_back(server);
}