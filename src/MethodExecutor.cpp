/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodExecutor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:30:23 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/17 13:57:14 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MethodExecutor.hpp"

MethodExecutor::MethodExecutor(Server& s, Request& r, std::string m) : _server(s), _request(r), _method(m)
{
    this->execute();
}

Location& MethodExecutor::getRequestLocation()
{
    std::string path(this->_request.getUri());
    std::map<std::string, Location>& locations = this->_server.getLocations();

    size_t  l = path.find_last_of('/');
    if (l == 0)
        path = "/";
    else if (l != std::string::npos)
        path = path.substr(0, l);
    while (1)
    {
        for (std::map<std::string, Location >::iterator it = locations.begin(); it != locations.end(); it++)
        {
            if (it->first == path)
                return it->second;            
        }
        if (path.empty() || path == "/")
            break ;

        l = path.find_last_of('/');
        if (l == 0)
            path = "/";
       else if (l != std::string::npos)
            path = path.substr(0, l);
        else
            break ;
    }

    Location*    error = new Location;
    error->setCode(404);
    return *error;
    
}

Response& MethodExecutor::getResponse()
{
    return this->_response;
}

void    MethodExecutor::execute()
{
    // erreur potentiel :
    // si pas de location
    // si fichier existe pas
    // si not allowed method
    // si pas de fichier dans l'url rediriger sur index le plus proche si pas d'index alors genere autoindex si pas autoindex 404 not found
    
    Location    loc;

    loc = this->getRequestLocation();
    
    std::string fileName(loc.getRoot());
    fileName += this->_request.getUri();

    if (this->_method == "GET" && std::find(loc.getAllowedMethods().begin(), loc.getAllowedMethods().end(), "GET") != loc.getAllowedMethods().end())
        this->_response = AHttpMethod::GET(fileName, loc, this->_request, this->_server);
    else if (this->_method == "POST" && std::find(loc.getAllowedMethods().begin(), loc.getAllowedMethods().end(), "POST") != loc.getAllowedMethods().end())
        this->_response = AHttpMethod::GET(fileName, loc, this->_request, this->_server);
    else if (this->_method == "DELETE" && std::find(loc.getAllowedMethods().begin(), loc.getAllowedMethods().end(), "DELETE") != loc.getAllowedMethods().end())
        this->_response = AHttpMethod::GET(fileName, loc, this->_request, this->_server);
    // std::cout << loc.getCode() << std::endl;
    // AHttpMethod::GET("index.html", loc);
}

MethodExecutor::~MethodExecutor() {}