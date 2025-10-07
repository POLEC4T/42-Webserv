/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:34:04 by dmazari           #+#    #+#             */
/*   Updated: 2025/10/07 16:10:29 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
# define ERROR_HPP

# include <exception>
# include <string>

class Error {
    public:
    	class CanNotOpenFile: public std::exception {
			private:
				std::string	_message;
			public:
				const char* what() const throw() ;

				CanNotOpenFile(const std::string&);
				~CanNotOpenFile() throw();
		};

		class ErrorBracketParseFile: public std::exception {
			public:
				const char* what() const throw() ;
		};

    	class NoPageFound: public std::exception {
			private:
				std::string	_message;
			public:
				const char* what() const throw() ;

				NoPageFound(const std::string&);
				~NoPageFound() throw() ;
		};

        class DidNotFindSemicolon: public std::exception {
			private:
				std::string	_message;
            public:
                const char* what() const throw() ;

				DidNotFindSemicolon(const std::string&);
				~DidNotFindSemicolon() throw() ;
        };

        class UnknownToken: public std::exception {
			private:
				std::string	_message;
            public:
                const char* what() const throw() ;
				
				UnknownToken(const std::string&);
				~UnknownToken() throw() ;
        };

		class IntExpected: public std::exception {
			private:
				std::string	_message;
			public:
				const char* what() const throw() ;
				
				IntExpected(const std::string&);
				~IntExpected() throw() ;
		};

};

#endif