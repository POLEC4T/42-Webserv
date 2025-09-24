/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMethod.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 23:37:42 by mazakov           #+#    #+#             */
/*   Updated: 2025/09/24 23:40:15 by mazakov          ###   ########.fr       */
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

#endif;
