/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 17:36:12 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/22 16:27:35 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Response.hpp"
#include "Location.hpp"

class CGIHandler {
private:
  Request &_request;
  Location& _location;

public:
  CGIHandler(const Request &req, const Location& loc);
  ~CGIHandler();
};
