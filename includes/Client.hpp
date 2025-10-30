/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 11:53:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/30 11:05:22 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Headers.h"
#include "Request.hpp"

typedef enum e_client_status { WAITING, READY } t_client_status;

class Client {
private:
	Request _request;
	std::string _recvBuffer;
	std::string _sendBuffer;
	size_t _sentIdx;
	t_client_status _status;
	int _fd;
	size_t checkAndGetContentLength(Server &serv,
									const std::string &contentLengthStr);

public:
	Client();
	Client(int fd);
	~Client();

	int getFd() const;
	const std::string &getBuffer() const;
	t_client_status getStatus() const;

	void setStatus(t_client_status status);
	void setSendBuffer(const std::string &buf);

	void appendBuffer(char *buffer);
	void appendBuffer(const char *buffer);
	void clearBuffer();
	bool receivedRequestLine() const;
	bool receivedHeaders() const;
	bool receivedBody(size_t contentLength) const;

	Request &getRequest();
	void parseRequest(Server &serv);
	void resetForNextRequest();

	int sendPendingResponse(int epollfd);
};

#endif