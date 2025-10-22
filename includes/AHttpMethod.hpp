/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:37:42 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/22 15:56:34 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AHTTPMETHOD_HPP
#define AHTTPMETHOD_HPP

#include "Headers.h"
#include "Location.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Location;

class AHttpMethod {
private:
  AHttpMethod();
  virtual ~AHttpMethod() = 0;

public:
  static Response GET(std::string, Location &, Request &, Server &);
  static Response DELETE(std::string, Request &, Server &);
  static Response POST(std::string filename, Location &loc, Request &req,
                       Server &serv);
};

#endif
