/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dorianmazari <dorianmazari@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 11:53:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/20 13:42:44 by dorianmazar      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include <string>

typedef enum e_client_status { WAITING, READY } t_client_status;

class Client {
private:
  std::string _buffer;
  t_client_status _status;
  int _fd;
  int _getContentLength() const;

public:
  Client();
  Client(int fd);
  ~Client();

  int getFd() const;
  const std::string &getBuffer() const;
  t_client_status getStatus() const;

  void setStatus(t_client_status status);

  void appendBuffer(char *buffer);
  void appendBuffer(const char *buffer);
  void clearBuffer();
  bool hasReceivedFullReq();
};

#endif