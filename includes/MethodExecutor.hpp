/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodExecutor.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dorianmazari <dorianmazari@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:25:48 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/20 13:42:11 by dorianmazar      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __METHODEXECUTOR_HPP__
#define __METHODEXECUTOR_HPP__

#include "Headers.h"
#include "Location.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include <algorithm>

class MethodExecutor {
private:
  Server &_server;
  Request _request;
  std::string _method;
  Response _response;
  int _clientFd;
  Location &getRequestLocation();

public:
  MethodExecutor(Server &, Request &, std::string, int clientFd);
  void execute();
  Response &getResponse();
  ~MethodExecutor();
};

#endif