/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodExecutor.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:25:48 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/22 17:02:52 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __METHODEXECUTOR_HPP__
#define __METHODEXECUTOR_HPP__

#include "Headers.h"
#include "Client.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

class   MethodExecutor
{
    private:
        Server &_server;
        Client _client;
        std::string _method;
        Request _request;
        Response    _response;

    public:
        MethodExecutor(Server&, Client&);
        void    execute();
        Response& getResponse();
        static Location&    getRequestLocation(Request&, Server&);
        ~MethodExecutor();
};

#endif