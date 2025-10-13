/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultErrorPage.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:19:03 by dmazari           #+#    #+#             */
/*   Updated: 2025/10/13 14:37:55 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULTERRORPAGE_HPP
# define DEFAULTERRORPAGE_HPP

# include "Headers.h"

class DefaultErrorPage : public APage {
    private:

    public:
        //Canonical
        DefaultErrorPage();
        DefaultErrorPage(const DefaultErrorPage& cpy);
        DefaultErrorPage&   operator=(const DefaultErrorPage& other);
        ~DefaultErrorPage();

        std::string parseFilesAndSetContents();
}

#endif