/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:34:04 by dmazari           #+#    #+#             */
/*   Updated: 2025/10/30 11:41:49 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>
#include <string>

class Error {
public:
	class CanNotOpenFile : public std::exception {
	private:
		std::string _message;

	public:
		const char *what() const throw();

		CanNotOpenFile(const std::string &);
		~CanNotOpenFile() throw();
	};

	class ErrorBracketParseFile : public std::exception {
	public:
		const char *what() const throw();
	};

	class NoPageFound : public std::exception {
	private:
		std::string _message;

	public:
		const char *what() const throw();

		NoPageFound(const std::string &);
		~NoPageFound() throw();
	};

	class DidNotFindSemicolon : public std::exception {
	private:
		std::string _message;

	public:
		const char *what() const throw();

		DidNotFindSemicolon(const std::string &);
		~DidNotFindSemicolon() throw();
	};

	class UnknownToken : public std::exception {
	private:
		std::string _message;

	public:
		const char *what() const throw();

		UnknownToken(const std::string &);
		~UnknownToken() throw();
	};

	class IntExpected : public std::exception {
	private:
		std::string _message;

	public:
		const char *what() const throw();

		IntExpected(const std::string &);
		~IntExpected() throw();
	};

	class IntOutOfRange : public std::exception {
	private:
		std::string _message;

	public:
	const char *what() const throw();
	
	IntOutOfRange(const std::string&);
	~IntOutOfRange() throw();
	};

	class NoServerInConfigFile : public std::exception {
	public:
		const char *what() const throw();
	};

	class NoRelatedServerFound: public std::exception {
		private:
			std::string	_message;
		public:
			const char* what() const throw() ;

			NoRelatedServerFound(int fd);
			~NoRelatedServerFound() throw() ;
	};

	class CgiValuesError: public std::exception {
		public:
			const char *what() const throw();
	};
};

#endif