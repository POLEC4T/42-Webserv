/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 12:19:19 by dorianmazar       #+#    #+#             */
/*   Updated: 2025/10/22 16:28:24 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"

CGIHandler::~CGIHandler() {}

std::vector<std::string> setEnvCGI(const Request &req) {
  std::vector<std::string> env;
  FtString uri = req.getUri();
  std::vector<std::string> args;

  args = uri.ft_split("?");
  if (args.size() == 2)
    env.push_back("QUERY_STRING=" + args[1]);
  else
    env.push_back("QUERY_STRING=");
  env.push_back("REQUEST_METHOD=" + req.getMethod());
  env.push_back("SCRIPT_PATH=" + args[0]);
  env.push_back("SERVER_PROTOCOL=HTTP/1.1");
  env.push_back("GATEWAY_INTERFACE=CGI/1.1");
  env.push_back("CONTENT_TYPE=" + req.getHeaderValue("content-type"));
  env.push_back("CONTENT_LENGHT=" + req.getHeaderValue("content-lenght"));
}

CGIHandler::CGIHandler(const Request& req, const Location& loc) {
  std::vector<std::string> env = setEnvCGI(req);
  if ()

}
