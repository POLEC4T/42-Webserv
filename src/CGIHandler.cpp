/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmazari <dmazari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 12:19:19 by dorianmazar       #+#    #+#             */
/*   Updated: 2025/10/27 16:49:31 by dmazari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MethodExecutor.hpp"
#include "sys/time.h"

#define TIMEDOUT 1

typedef struct s_CGIContext {
  char **env;
  char **args;
  int pipeFdIn[2];
  int pipeFdOut[2];
  int pid;
  int status;
  int timedOut;
} t_CGIContext;

bool isCGI(Request &req, Location &loc) {
  FtString token = req.getUri();
  std::string method = req.getMethod();

  if (method != "POST" && method != "GET")
    return false;
  if (loc.getCgiExtension().empty() || loc.getCgiPath().empty())
    return false;
  std::vector<std::string> parts = token.ft_split("?");
  const std::string &pathOnly = parts.size() > 0 ? parts[0] : std::string();
  size_t extensionIndex = pathOnly.find('.');
  if (extensionIndex == std::string::npos)
    return false;
  std::string extension =
      pathOnly.substr(extensionIndex, pathOnly.size() - extensionIndex);
  if (extension == loc.getCgiExtension())
    return true;
  return false;
}

std::vector<std::string> setEnvCGI(std::vector<std::string> tokens,
                                   Request &req) {
  std::vector<std::string> env;

  if (tokens.size() == 2)
    env.push_back("QUERY_STRING=" + tokens[1]);
  env.push_back("REQUEST_METHOD=" + req.getMethod());
  env.push_back("SCRIPT_PATH=" + tokens[0]);
  env.push_back("SERVER_PROTOCOL=HTTP/1.1");
  env.push_back("GATEWAY_INTERFACE=CGI/1.1");
  env.push_back("CONTENT_TYPE=" + req.getHeaderValue("Content-Type"));
  env.push_back("CONTENT_LENGTH=" + req.getHeaderValue("Content-Length"));
  return (env);
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

int getContext(t_CGIContext &ctx, Location &loc, Request &req) {
  std::vector<std::string> envVec;
  std::vector<std::string> argsVec;
  std::vector<std::string> tokens;
  FtString token;

  token = req.getUri();
  tokens = token.ft_split("?");
  envVec = setEnvCGI(tokens, req);
  argsVec.push_back(loc.getCgiPath());
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

void initCGIContext(t_CGIContext &ctx) {
  ctx.args = NULL;
  ctx.env = NULL;
  ctx.pipeFdIn[0] = -1;
  ctx.pipeFdIn[1] = -1;
  ctx.pipeFdOut[0] = -1;
  ctx.pipeFdOut[1] = -1;
  ctx.status = 0;
  ctx.timedOut = 0;
}

void closeFdOfContext(t_CGIContext &ctx) {
  ftClose(&ctx.pipeFdIn[0]);
  ftClose(&ctx.pipeFdIn[1]);
  ftClose(&ctx.pipeFdOut[0]);
  ftClose(&ctx.pipeFdOut[1]);
}

void freeCGIContext(t_CGIContext &ctx) {
  if (ctx.args)
    freeCharArray(ctx.args);
  if (ctx.env)
    freeCharArray(ctx.env);
  closeFdOfContext(ctx);
}

std::string readToHTTPBody(int fd) {
  int bytes = 0;
  std::string content;
  char buffer[20];

  while (1) {
    bytes = read(fd, buffer, 19);
    if (bytes == -1)
      return std::string();
    if (bytes == 0)
      break;
    buffer[bytes] = '\0';
    content += buffer;
  }
  return content;
}

void handle_alarm(int signo) {
  (void)signo;
}


int  timedOutHandling(t_CGIContext& ctx, int timedOut) {
  int waitPidRet;
  itimerval timer_value;
  errno = 0;

  struct sigaction sa;
  sa.sa_handler = handle_alarm;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGALRM, &sa, NULL);

  timer_value.it_value.tv_sec = 0;
  if (timedOut == -1)
    timer_value.it_value.tv_usec = 500000;
  else
    timer_value.it_value.tv_usec = timedOut;
  timer_value.it_interval.tv_sec = 0;
  timer_value.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &timer_value, NULL);
  waitPidRet = waitpid(ctx.pid, &ctx.status, 0);
  if (waitPidRet > 0)
    alarm(0);
  else if (waitPidRet == -1 && errno == EINTR) {
    kill(ctx.pid, SIGKILL);
    waitpid(ctx.pid, NULL, 0);
    return (TIMEDOUT);
  }
  return (EXIT_SUCCESS);
}

int executeChild(t_CGIContext ctx) {
  if (ctx.pipeFdIn[0] != -1) {
    if (dup2(ctx.pipeFdIn[0], STDIN_FILENO) == -1) {
      freeCGIContext(ctx);
	  std::cerr << "CGI: dup2 pipeFdIn[0] error" << std::endl;
      std::exit(1);
    }
  }
  if (dup2(ctx.pipeFdOut[1], STDOUT_FILENO) == -1 ||
     dup2(ctx.pipeFdOut[1], STDERR_FILENO) == -1) {
    std::cerr << "CGI: dup2 pipeFdOut[1] error" << std::endl;
    freeCGIContext(ctx);
    std::exit(1);
  }
  closeFdOfContext(ctx);
  execve(ctx.args[0], ctx.args, ctx.env);
  std::cerr << "CGI: execve error" << std::endl;
  std::exit(1);
}

Response CGIHandler(Request &req, Location &loc, Server &serv) {
  t_CGIContext ctx;
  std::string method = req.getMethod();
  FtString uriToken = req.getUri();
  std::vector<std::string> uriParts = uriToken.ft_split("?");
  std::string scriptPath =
      uriParts.size() ? loc.getRoot() + uriParts[0] : std::string();

  initCGIContext(ctx);
  if (scriptPath.empty() || access(loc.getCgiPath().c_str(), X_OK) == -1 ||
      access(scriptPath.c_str(), R_OK) == -1) {
    std::cerr << "CGI: Access error" << std::endl;
    return Response(req.getVersion(), serv.getErrorPageByCode(BAD_REQUEST));
  }
  if (getContext(ctx, loc, req) || pipe(ctx.pipeFdOut)) {
    freeCGIContext(ctx);
    std::cerr << "CGI: Get context error" << std::endl;
    return Response(req.getVersion(),
                    serv.getErrorPageByCode(INTERNAL_SERVER_ERROR));
  }
  if (method == "POST") {
    if (pipe(ctx.pipeFdIn)) {
      freeCGIContext(ctx);
      std::cerr << "CGI: pipe Post method error" << std::endl;
      return Response(req.getVersion(),
                      serv.getErrorPageByCode(INTERNAL_SERVER_ERROR));
    }
    write(ctx.pipeFdIn[1], req.getBody().c_str(), req.getBody().size());
    ftClose(&ctx.pipeFdIn[1]);
  }
  ctx.pid = fork();
  if (ctx.pid == -1) {
    freeCGIContext(ctx);
    std::cerr << "CGI: fork error" << std::endl;
    return Response(req.getVersion(),
                    serv.getErrorPageByCode(INTERNAL_SERVER_ERROR));
  }
  if (ctx.pid == 0) {
    executeChild(ctx);
  }
  else {
    ftClose(&ctx.pipeFdOut[1]);
    ctx.timedOut = timedOutHandling(ctx, serv.getTimedOutValue());
  }
  if (ctx.timedOut == TIMEDOUT) {
    std::cerr << "CGI: timed out" << std::endl;
    return Response(req.getVersion(),
                    serv.getErrorPageByCode(REQUEST_TIMEOUT));
  }
  if (!WIFEXITED(ctx.status)) {
    freeCGIContext(ctx);
    std::cerr << "CGI: child exit error" << std::endl;
    return Response(req.getVersion(),
                    serv.getErrorPageByCode(INTERNAL_SERVER_ERROR));
  }
  std::string content = readToHTTPBody(ctx.pipeFdOut[0]);
  freeCGIContext(ctx);
  if (content.empty()) {
    std::cerr << "CGI: read from CGI return error" << std::endl;
    return Response(req.getVersion(),
                    serv.getErrorPageByCode(INTERNAL_SERVER_ERROR));
  }
  return Response(req.getVersion(), OK, "OK", content);
}
