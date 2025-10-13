/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 17:37:50 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/13 18:05:25 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        Response(std::string, int, std::string);
        Response& operator=(const Response& other);
        ~Response();
        std::string build();
        void    setBody(const std::string& body);
        void    setHeader(const std::string& key, const std::string& value);
};
