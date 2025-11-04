/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 17:37:50 by faoriol           #+#    #+#             */
/*   Updated: 2025/11/01 22:32:37 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include "ErrorPage.hpp"
# include <map>

class Response
{
	private:
		std::string _version;
		int _code;
		std::string _status;
		std::map<std::string, std::string> _headers;
		std::string _body;

	public:
		Response();
		Response(std::string, ErrorPage &);
		Response(std::string, ErrorPage &, std::string);
		Response(std::string, int, std::string, std::string);
		Response &operator=(const Response &other);
		~Response();
		std::string build();
		void setBody(const std::string &body);
		void setHeader(const std::string &key, const std::string &value);

		std::string getVersion() const;
		int getCode() const;
		std::string getStatus() const;
		std::string getBody() const;
		std::map<std::string, std::string> getHeaders() const;
};

#endif
