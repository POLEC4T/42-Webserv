/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mniemaz <mniemaz@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 11:53:19 by mniemaz           #+#    #+#             */
/*   Updated: 2025/11/06 11:46:32 by mniemaz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Request.hpp"
# include <string>

# define MAX_RECV 8192
# define CRLF_SIZE 2
# define LINGERING 2

/**
 * Linger exists because if we delete the client (and close socket) right after 
 * sending the 413 (Content too large) response, the client won't have time to
 * read that response because it may still be sending the big content. So we let it
 * "linger", in other words we read then throw away what the client is sending.
 * We do that for _lingerQuota bytes. If _lingerQuota is not reached, the client
 * gets deleted after _lingerDeadline time.
 */
typedef enum e_client_status {
	WAITING, 	// Waiting for other packets
	READY,		// Ready to send response
	LINGER		// Connection will be closed after receiving a few more packets, so that the client have the time read the response
} t_client_status;

class Client {
	private:
		/**
		 * While reading the chunk, we can't know if we received the whole.
		 * So, we need to keep track of what we were doing before stopping
		 * due to lack of data. So that we can resume properly.
		 */
		typedef enum e_current_chunk_part {
			SIZE,	// Reading chunk size line
			DATA	// Reading chunk data line
		} t_current_chunk_part;

		typedef enum e_chunk_state {
			CHUNK_INCOMPLETE,		// Need more data to proceed
			CHUNK_COMPLETE,			// Chunk part (size or data) complete
			CHUNK_FINAL_COMPLETE	// Parsed final zero-size chunk, all done
		} t_chunk_state;

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
		bool					_deleteClientAfterResponse;
		bool					_enableLingerAfterResponse;
		ssize_t					_lingerQuota; // max octets to read before deleting the client
		time_t					_lingerDeadline; // time after which the client has to be deleted
		size_t					_checkAndGetContentLength(const std::string& contentLengthStr);
		t_chunk_state			_parseChunkData(const std::string& chunks, size_t& pos);
		t_chunk_state			_parseChunkSize(const std::string& chunks, size_t& pos);

	public:
		Client();
		Client(int fd);
		~Client();

		int					getFd() const;
		t_client_status		getStatus() const;
		const std::string&	getRecvBuffer() const;
		bool				getdeleteClientAfterResponse() const;
		time_t				getLingerDeadline() const;

		void				setStatus(t_client_status status);
		void				setSendBuffer(const std::string &buf);
		void				setdeleteClientAfterResponse(bool);

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