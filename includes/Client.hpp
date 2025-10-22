/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faoriol <faoriol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 11:53:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/22 16:59:14 by faoriol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Headers.h"
#include "Request.hpp"

typedef enum e_client_status {
	WAITING,
	READY
} t_client_status;

class Client {
	private:
		Request			_request;
		std::string		_recvBuffer;
		std::string		_sendBuffer;
		size_t			_sentIdx;
		t_client_status	_status;
		int 			_fd;
		size_t checkAndGetContentLength(const std::string& contentLengthStr) const;
		

	public:
		Client();
		Client(int fd);
		~Client();

		int					getFd() const;
		const std::string&	getBuffer() const;
		t_client_status		getStatus() const;

		void				setStatus(t_client_status status);
		void				setSendBuffer(const std::string& buf);

		void				appendBuffer(char *buffer);
		void				appendBuffer(const char *buffer);
		void				clearBuffer();
		bool				receivedRequestLine() const;
		bool				receivedHeaders() const;
		bool				receivedBody(size_t contentLength) const;

		Request&			getRequest();
		void				parseRequest();
		void 				resetForNextRequest();

		int					sendPendingResponse(int epollfd);
};

#endif