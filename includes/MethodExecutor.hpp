/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodExecutor.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:25:48 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/17 17:38:57 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __METHODEXECUTOR_HPP__
#define __METHODEXECUTOR_HPP__

#include "Headers.h"
#include "Response.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Location.hpp"
#include <algorithm>

class   MethodExecutor
{
    private:
        Server &_server;
        Request _request;
        std::string _method;
        Response    _response;
        Location&    getRequestLocation();

    public:
        MethodExecutor(Server&, Request&, std::string);
        void    execute();
        Response& getResponse();
        // Reponse& getResponse();
        ~MethodExecutor();
};

#endif