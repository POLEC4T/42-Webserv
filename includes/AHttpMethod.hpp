/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dorianmazari <dorianmazari@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:37:42 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/27 15:03:44 by dorianmazar      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	AHTTPMETHOD_HPP
# define AHTTPMETHOD_HPP

class	AHttpMethod {
	private:

	public:
		AHttpMethod();
		AHttpMethod(const AHttpMethod& cpy);
		AHttpMethod& operator=(const AHttpMethod& other);
		virtual ~AHttpMethod() = 0;
};

#endif
