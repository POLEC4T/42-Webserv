/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 12:19:19 by dorianmazar       #+#    #+#             */
/*   Updated: 2025/11/03 16:15:26 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"
#include "MethodExecutor.hpp"
#include "defines.h"
#include "epoll.hpp"
#include "Error.hpp"
#include <ctime>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define TIMEDOUT 1

typedef struct s_CGIContext {
	std::string cgiPath;
	char** env;
	char** args;
	int pipeFdIn[2];
	int pipeFdOut[2];
	int pid;
	int status;
	int timedOut;
} t_CGIContext;

std::string getCgiExtensionInUri(Request& req) {
	FtString token = req.getUri();
	std::vector<std::string> parts = token.ft_split("?");
	const std::string& pathOnly = parts.size() > 0 ? parts[0] : std::string();
	size_t extensionIndex = pathOnly.find('.');

	if (extensionIndex == std::string::npos)
		return "";
	std::string extension =
		pathOnly.substr(extensionIndex, pathOnly.size() - extensionIndex);
	return extension;
}

bool isCGI(Request& req, Location& loc) {
	std::string method = req.getMethod();
	std::string extension = getCgiExtensionInUri(req);

	if (extension.empty())
		return false;
	if (method != "POST" && method != "GET")
		return false;
	if (!loc.getCgiByExtension(extension).empty())
		return true;
	return false;
}

char** vectorToCharArray(std::vector<std::string> vec) {
	char** strs;

	strs = new (std::nothrow) char* [vec.size() + 1];
	if (!strs)
		return NULL;
	for (size_t i = 0; i < vec.size(); i++) {
		strs[i] = new (std::nothrow) char[vec[i].size() + 1];
		if (!strs[i]) {
			for (size_t j = 0; j < i; j++) {
				delete[] strs[j];
			}
			delete[] strs;
			return NULL;
		}
		std::copy(vec[i].c_str(), vec[i].c_str() + vec[i].size() + 1, strs[i]);
	}
	strs[vec.size()] = NULL;
	return strs;
}

void freeCharArray(char** strs) {
	for (size_t i = 0; strs && strs[i]; i++) {
		delete[] strs[i];
	}
	if (strs)
		delete[] strs;
}

void ftClose(int* fd) {
	if (*fd != -1)
		close(*fd);
	*fd = -1;
}

std::vector<std::string> setEnvCGI(std::vector<std::string> tokens,
								Request &req, Server &serv) {
	std::vector<std::string> env;

	if (tokens.size() == 2)
		env.push_back("QUERY_STRING=" + tokens[1]);

	env.push_back("SERVER_NAME=" + serv.getNames()[0]);
	env.push_back("SERVER_PORT=" + serv.getPorts()[0]);
	env.push_back("REQUEST_METHOD=" + req.getMethod());
	env.push_back("SCRIPT_NAME=" + tokens[0]);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_SOFTWARE=PixelVerse/1.0");
	env.push_back("REMOTE_ADDR=" + serv.getHost());

	if (!req.getHeaderValue("Content-Type").empty())
		env.push_back("CONTENT_TYPE=" + req.getHeaderValue("Content-Type"));
	if (!req.getBody().empty())
		env.push_back("CONTENT_LENGTH=" + FtString::my_to_string(req.getBody().size()));

	env.push_back("PATH_INFO=");
	env.push_back("PATH_TRANSLATED=");

	std::string cookie = req.getHeaderValue("Cookie");
	if (!cookie.empty())
		env.push_back("HTTP_COOKIE=" + cookie);
	return (env);
}

void initCGIContext(t_CGIContext& ctx) {
	ctx.args = NULL;
	ctx.env = NULL;
	ctx.pipeFdIn[0] = -1;
	ctx.pipeFdIn[1] = -1;
	ctx.pipeFdOut[0] = -1;
	ctx.pipeFdOut[1] = -1;
	ctx.status = 0;
	ctx.timedOut = 0;
}

int getContext(t_CGIContext& ctx, Location& loc, Request& req, Server& serv) {
	std::vector<std::string> envVec;
	std::vector<std::string> argsVec;
	std::vector<std::string> tokens;
	FtString token;

	initCGIContext(ctx);
	ctx.cgiPath = loc.getCgiByExtension(getCgiExtensionInUri(req));
	token = req.getUri();
	tokens = token.ft_split("?");
	envVec = setEnvCGI(tokens, req, serv);
	argsVec.push_back(ctx.cgiPath);
	argsVec.push_back(loc.getRoot() + tokens[0]);
	ctx.env = vectorToCharArray(envVec);
	if (!ctx.env)
		return (EXIT_FAILURE);
	ctx.args = vectorToCharArray(argsVec);
	if (!ctx.args) {
		freeCharArray(ctx.env);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int initCgiPipes(t_CGIContext& ctx) {
	if (pipe(ctx.pipeFdOut) || pipe(ctx.pipeFdIn))
		return EXIT_FAILURE;
	if (fcntl(ctx.pipeFdOut[0], F_SETFL, O_NONBLOCK) == -1)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

void closeFdOfContext(t_CGIContext& ctx) {
	ftClose(&ctx.pipeFdIn[0]);
	ftClose(&ctx.pipeFdIn[1]);
	close(ctx.pipeFdOut[0]);
	ftClose(&ctx.pipeFdOut[1]);
}

void freeCGIContext(t_CGIContext& ctx) {
	if (ctx.args)
		freeCharArray(ctx.args);
	if (ctx.env)
		freeCharArray(ctx.env);
	closeFdOfContext(ctx);
}

void freeCGIContextMainProcess(t_CGIContext &ctx) {
	if (ctx.args)
		freeCharArray(ctx.args);
	if (ctx.env)
		freeCharArray(ctx.env);
	ftClose(&ctx.pipeFdIn[0]);
	ftClose(&ctx.pipeFdIn[1]);
	ftClose(&ctx.pipeFdOut[1]);
}

int executeChild(t_CGIContext ctxCGI) {
	if (1) {
		freeCGIContext(ctxCGI);
		std::cerr << "CGI: dup2 pipeFdIn[0] error" << std::endl;
		throw (Error::ErrorCGI(INTERNAL_SERVER_ERROR, ctxCGI.pid, ctxCGI.pipeFdOut[0]));
	}
	if (dup2(ctxCGI.pipeFdOut[1], STDOUT_FILENO) == -1 || dup2(ctxCGI.pipeFdOut[1], STDERR_FILENO) == -1) {
		std::cerr << "CGI: dup2 pipeFdOut[1] error" << std::endl;
		freeCGIContext(ctxCGI);
		throw (Error::ErrorCGI(INTERNAL_SERVER_ERROR, ctxCGI.pid, ctxCGI.pipeFdOut[0]));
	}
	closeFdOfContext(ctxCGI);
	execve(ctxCGI.args[0], ctxCGI.args, ctxCGI.env);
	freeCGIContext(ctxCGI);
	std::cerr << "CGI: execve error" << std::endl;
	throw (Error::ErrorCGI(INTERNAL_SERVER_ERROR, ctxCGI.pid, ctxCGI.pipeFdOut[0]));
}

int CGIHandler(Request &req, Location &loc, Server &serv, Client &client,
			Context &ctx) {
	t_CGIContext cgiCtx;
	std::string content;
	std::string method = req.getMethod();
	FtString uriToken = req.getUri();
	std::vector<std::string> uriParts = uriToken.ft_split("?");
	std::string scriptPath =
		uriParts.size() ? loc.getRoot() + uriParts[0] : std::string();

    
	if (getContext(cgiCtx, loc, req, serv)) {
		freeCGIContext(cgiCtx);
		std::cerr << "CGI: Get context error" << std::endl;
		return INTERNAL_SERVER_ERROR;
	}

	if (scriptPath.empty() || access(cgiCtx.cgiPath.c_str(), X_OK) == -1 ||
		access(scriptPath.c_str(), R_OK) == -1) {
		std::cerr << "CGI: Access error" << std::endl;
		return BAD_REQUEST;
	}

	if (initCgiPipes(cgiCtx)) {
		freeCGIContext(cgiCtx);
		std::cerr << "CGI: Pipe error" << std::endl;
		return INTERNAL_SERVER_ERROR;
	}

	write(cgiCtx.pipeFdIn[1], client.getRecvBuffer().c_str(),
		req.getBody().size());
	ftClose(&cgiCtx.pipeFdIn[1]);
	cgiCtx.pid = fork();

	if (cgiCtx.pid == -1) {
		freeCGIContext(cgiCtx);
		std::cerr << "CGI: fork error" << std::endl;
		return INTERNAL_SERVER_ERROR;
	}

	if (cgiCtx.pid == 0) {
		executeChild(cgiCtx);	
	} else {
    	ftClose(&cgiCtx.pipeFdOut[1]);

		if (my_epoll_ctl(ctx.getEpollFd(), EPOLL_CTL_ADD, EPOLLIN, cgiCtx.pipeFdOut[0]) == -1) {
			freeCGIContext(cgiCtx);
			return DELETE_CLIENT;
		}

		freeCGIContextMainProcess(cgiCtx);

		CGI cgi(serv, client);

		cgi.setFd(cgiCtx.pipeFdOut[0]);
		cgi.setPid(cgiCtx.pid);

		ctx.addCgi(cgi);

		return CGI_PENDING;
	}
	return INTERNAL_SERVER_ERROR;
}

	// if (ctx.timedOut == TIMEDOUT) {
	// 	std::cerr << "CGI: timed out." << std::endl;
	// 	return Response(req.getVersion(),
	// 					serv.getErrorPageByCode(REQUEST_TIMEOUT))
	// 		.build();
	// }

	// if (!WIFEXITED(ctx.status)) {
	// 	freeCGIContext(ctx);
	// 	std::cerr << "CGI: child exit error" << std::endl;
	// 	return Response(req.getVersion(),
	// 					serv.getErrorPageByCode(INTERNAL_SERVER_ERROR))
	// 		.build();
	// }

	// freeCGIContext(ctx);

	// if (content.empty()) {
	// 	std::cerr << "CGI: read from CGI return error" << std::endl;
	// 	return Response(req.getVersion(),
	// 					serv.getErrorPageByCode(INTERNAL_SERVER_ERROR))
	// 		.build();
	// }
	// return content;

