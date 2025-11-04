/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 12:19:19 by dorianmazar       #+#    #+#             */
/*   Updated: 2025/11/04 15:54:56 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Context.hpp"
#include "Error.hpp"
#include "MethodExecutor.hpp"
#include "defines.h"
#include "epoll.hpp"
#include <ctime>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define TIMEDOUT 1

typedef struct s_CGIContext {
	std::string cgiPath;
	char **env;
	char **args;
	int pipeInputCGI[2];
	int pipeOutputCGI[2];
	int pid;
	int status;
	int timedOut;
} t_CGIContext;

std::string getCgiExtensionInUri(Request &req) {
	FtString token = req.getUri();
	std::vector<std::string> parts = token.ft_split("?");
	const std::string &pathOnly = parts.size() > 0 ? parts[0] : std::string();
	size_t extensionIndex = pathOnly.find('.');

	if (extensionIndex == std::string::npos)
		return "";
	std::string extension =
		pathOnly.substr(extensionIndex, pathOnly.size() - extensionIndex);
	return extension;
}

bool isCGI(Request &req, Location &loc) {
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

char **vectorToCharArray(std::vector<std::string> vec) {
	char **strs;

	strs = new (std::nothrow) char *[vec.size() + 1];
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

void freeCharArray(char **strs) {
	for (size_t i = 0; strs && strs[i]; i++) {
		delete[] strs[i];
	}
	if (strs)
		delete[] strs;
}

void ftClose(int *fd) {
	if (*fd != -1)
		close(*fd);
	*fd = -1;
}

std::vector<std::string> setEnvCGI(std::vector<std::string> tokens,
								Request &req, Server &serv, Location &loc) {
	std::vector<std::string> env;

	if (tokens.size() == 2)
		env.push_back("QUERY_STRING=" + tokens[1]);
	env.push_back("SERVER_PORT=" + serv.getPorts()[0]);
	std::string serverName = serv.getNames().size() > 0 ? serv.getNames()[0] : "localhost";
	env.push_back("SERVER_NAME=" + serverName);
	env.push_back("REQUEST_METHOD=" + req.getMethod());
	env.push_back("SCRIPT_NAME=" + tokens[0]);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_SOFTWARE=PixelVerse/1.0");
	env.push_back("REMOTE_ADDR=" + serv.getHost());

	if (!req.getHeaderValue("Content-Type").empty())
		env.push_back("CONTENT_TYPE=" + req.getHeaderValue("Content-Type"));
	if (!req.getBody().empty())
		env.push_back("CONTENT_LENGTH=" +
					FtString::my_to_string(req.getBody().size()));

	env.push_back("PATH_INFO=" + tokens[0]);
	env.push_back("PATH_TRANSLATED=" + loc.getRoot());

	std::string cookie = req.getHeaderValue("Cookie");
	if (!cookie.empty())
		env.push_back("HTTP_COOKIE=" + cookie);
	return (env);
}

void initCGIContext(t_CGIContext &ctx) {
	ctx.args = NULL;
	ctx.env = NULL;
	ctx.pipeInputCGI[0] = -1;
	ctx.pipeInputCGI[1] = -1;
	ctx.pipeOutputCGI[0] = -1;
	ctx.pipeOutputCGI[1] = -1;
	ctx.status = 0;
	ctx.timedOut = 0;
}

int getContext(t_CGIContext &ctx, Location &loc, Request &req, Server &serv) {
	std::vector<std::string> envVec;
	std::vector<std::string> argsVec;
	std::vector<std::string> tokens;
	FtString token;

	initCGIContext(ctx);
	ctx.cgiPath = loc.getCgiByExtension(getCgiExtensionInUri(req));
	token = req.getUri();
	tokens = token.ft_split("?");
	envVec = setEnvCGI(tokens, req, serv, loc);
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

int initCgiPipes(t_CGIContext &ctx) {
	if (pipe(ctx.pipeOutputCGI) || pipe(ctx.pipeInputCGI))
		return EXIT_FAILURE;
	if (fcntl(ctx.pipeOutputCGI[0], F_SETFL, O_NONBLOCK) == -1)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

void closeFdOfContext(t_CGIContext &ctx) {
	ftClose(&ctx.pipeInputCGI[0]);
	ftClose(&ctx.pipeInputCGI[1]);
	ftClose(&ctx.pipeOutputCGI[0]);
	ftClose(&ctx.pipeOutputCGI[1]);
}

void freeCGIContext(t_CGIContext &ctx) {
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
	ftClose(&ctx.pipeInputCGI[0]);
	ftClose(&ctx.pipeInputCGI[1]);
	ftClose(&ctx.pipeOutputCGI[1]);
}

void executeChild(t_CGIContext ctxCGI) {
	if (dup2(ctxCGI.pipeInputCGI[0], STDIN_FILENO) == -1) {
		freeCGIContext(ctxCGI);
		std::cerr << "CGI: dup2 pipeInputCGI[0] error" << std::endl;
		throw(Error::ErrorCGI());
	}
	if (dup2(ctxCGI.pipeOutputCGI[1], STDOUT_FILENO) == -1) {
		std::cerr << "CGI: dup2 pipeOutputCGI[1] error" << std::endl;
		freeCGIContext(ctxCGI);
		throw(Error::ErrorCGI());
	}
	closeFdOfContext(ctxCGI);
	execve(ctxCGI.args[0], ctxCGI.args, ctxCGI.env);
	freeCGIContext(ctxCGI);
	std::cerr << "CGI: execve error" << std::endl;
	throw(Error::ErrorCGI());
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

	write(cgiCtx.pipeInputCGI[1], req.getBody().c_str(),
		req.getBody().size());
	ftClose(&cgiCtx.pipeInputCGI[1]);
	cgiCtx.pid = fork();

	if (cgiCtx.pid == -1) {
		freeCGIContext(cgiCtx);
		std::cerr << "CGI: fork error" << std::endl;
		return INTERNAL_SERVER_ERROR;
	}

	if (cgiCtx.pid == 0) {
		executeChild(cgiCtx);
	} else {
		ftClose(&cgiCtx.pipeOutputCGI[1]);

		if (my_epoll_ctl(ctx.getEpollFd(), EPOLL_CTL_ADD, EPOLLIN,
						cgiCtx.pipeOutputCGI[0]) == -1) {
			freeCGIContext(cgiCtx);
			return DELETE_CLIENT;
		}

		freeCGIContextMainProcess(cgiCtx);

		CGI cgi(serv, client, req);

		cgi.setFd(cgiCtx.pipeOutputCGI[0]);
		cgi.setPid(cgiCtx.pid);

		ctx.addCgi(cgi);

		return CGI_PENDING;
	}
	return INTERNAL_SERVER_ERROR;
}
