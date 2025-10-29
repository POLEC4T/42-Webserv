/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 11:53:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/10/29 10:31:22 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP


# include "Request.hpp"
# include <string>

# define MAX_RECV 8192
# define CRLF_SIZE 2

typedef enum e_client_status {
	WAITING, 	// Waiting for other packets
	READY		// Ready to send response
} t_client_status;

class Client {
	private:

		/**
		 * While reading the chunk, we can't know if we received the whole.
		 * So, we need to keep track of what we were doing before stopping
		 * due to lack of data. So that we can resume properly.
		 */
		typedef enum e_current_chunk_part {
			SIZE,
			DATA
		} t_current_chunk_part;

		Request					_request;
		std::string				_recvBuffer;
		std::string				_sendBuffer;
		size_t					_sentIdx;
		t_client_status			_status;
		int 					_fd;
		t_current_chunk_part	_currChunkPart;
		std::string				_chunks;
		size_t					_parsedChunksIdx;
		size_t					_currChunkSize;
		long long				_maxBodySize;
		long long				_contentLength;
		size_t					checkAndGetContentLength(const std::string& contentLengthStr);
		

	public:
		Client();
		Client(int fd);
		~Client();

		int					getFd() const;
		t_client_status		getStatus() const;

		void				setStatus(t_client_status status);
		void				setSendBuffer(const std::string& buf);

		bool				receivedRequestLine() const;
		bool				receivedHeaders() const;
		bool				receivedBody(size_t contentLength) const;

		Request&			getRequest();
		void				parsePacket(Server& serv);
		void 				resetForNextRequest();

		int					readPacket();
		int					sendPendingResponse(int epollfd);
		bool				unchunkBody(const std::string& chunks);
};

#endif