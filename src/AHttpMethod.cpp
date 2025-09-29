/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dorianmazari <dorianmazari@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 15:39:10 by dorianmazar       #+#    #+#             */
/*   Updated: 2025/09/28 15:47:09 by dorianmazar      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHttpMethod.hpp"

AHttpMethod::AHttpMethod() {}

AHttpMethod::AHttpMethod(const AHttpMethod& cpy) {
	(void)cpy;
}

AHttpMethod& AHttpMethod::operator=(const AHttpMethod& other) {
	(void)other;
	return *this;
}

AHttpMethod::~AHttpMethod() {}
