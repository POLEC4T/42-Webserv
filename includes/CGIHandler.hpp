/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 17:36:12 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/13 18:06:10 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Response.hpp"

class CGIHandler
{
    private:
        const Request& _request;
    public:
        CGIHandler(const Request& req);
        
        ~CGIHandler();
};
