/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultErrorPage.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:28:42 by dmazari           #+#    #+#             */
/*   Updated: 2025/10/13 14:40:29 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultErrorPage.hpp"

//Canonical

DefaultErrorPage::DefaultErrorPage() : APage() {}

DefaultErrorPage::DefaultErrorPage(const DefaultErrorPage& cpy) : APage(cpy) {}

DefaultErrorPage& DefaultErrorPage::operator=(const DefaultErrorPage& other) : APage(other) {}

DefaultErrorPage::~DefaultErrorPage() {}

DefaultErrorPage::parseFilesAndSetContents() {
    std::string path = "../errorPages/default";
}