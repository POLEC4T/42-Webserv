/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:37:42 by mazakov           #+#    #+#             */
/*   Updated: 2025/10/18 16:04:21 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	AHTTPMETHOD_HPP
# define AHTTPMETHOD_HPP

#include "Headers.h"
#include "Response.hpp"
#include "Location.hpp"
#include "Request.hpp"

class Location;

class	AHttpMethod {
	private:
		AHttpMethod();
		virtual ~AHttpMethod() = 0;
		
	public:
		static Response	GET(std::string, Location&, Request&, Server&);
		static Response	DELETE(std::string, Request&, Server&);
		// Response	POST(std::string, Location);
		// Response	DELETE(std::string, Location);
};

#endif
