/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodExecutor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazakov <mazakov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:30:23 by faoriol           #+#    #+#             */
/*   Updated: 2025/10/24 16:09:06 by mazakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MethodExecutor.hpp"

std::string readPage(std::string fileName);

MethodExecutor::MethodExecutor(Server &s, Client &c) : _server(s), _client(c) {
  this->_request = this->_client.getRequest();
  this->_method = this->_request.getMethod();
  this->execute();
}

Location &MethodExecutor::getRequestLocation(Request &req, Server &serv) {
  std::string path(req.getUri());
  std::map<std::string, Location> &locations = serv.getLocations();

  std::map<std::string, Location>::iterator it = locations.find(path);
  if (it != locations.end())
    return it->second;
  size_t l = path.find_last_of('/');
  if (l == 0)
    path = "/";
  else if (l != std::string::npos)
    path = path.substr(0, l);
  while (1) {
    for (std::map<std::string, Location>::iterator it = locations.begin();
         it != locations.end(); it++) {
      if (it->first == path)
        return it->second;
    }
    if (path.empty() || path == "/")
      break;

    l = path.find_last_of('/');
    if (l == 0)
      path = "/";
    else if (l != std::string::npos)
      path = path.substr(0, l);
    else
      break;
  }

  Location *error = new Location();
  error->setCode(PAGE_NOT_FOUND);
  return *error;
}

Response &MethodExecutor::getResponse() { return this->_response; }

int returnHandler(Response &response, Location &loc, Request &req,
                  Server &serv) {
  if (loc.getReturn().size() == 0)
    return 1;

  std::vector<std::string> split =
      static_cast<FtString>(loc.getReturn()).ft_split(" ");
  int i = 0;
  std::istringstream stream(split[0]);
  stream >> i;
  if (!stream.eof() || (i < 300 || i > 308)) {
    response = Response(req.getVersion(), serv.getErrorPageByCode(BAD_REQUEST));
    return 0;
  }

  response = Response(req.getVersion(), std::atoi(split[0].c_str()),
                      "Redirection", "");
  response.setHeader("Location", split[1]);
  return 0;
}

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
  env.push_back("CONTENT_TYPE=" + req.getHeaderValue("content-type"));
  env.push_back("CONTENT_LENGTH=" + req.getHeaderValue("content-length"));
  return (env);
}

int executeChild(int inputFd, int outputFd, char **args, char **env) {
  if (inputFd != 0) {
    if (dup2(inputFd, STDIN_FILENO) == -1)
      std::exit(1);
    close(inputFd);
  }
  if (dup2(outputFd, STDOUT_FILENO) == -1)
    std::exit(1);
  close(outputFd);
  execve(args[0], args, env);
  std::exit(1);
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

int getContext(char ***env, char ***args, Location &loc, Request &req) {
  std::vector<std::string> envVec;
  std::vector<std::string> argsVec;
  std::vector<std::string> tokens;
  FtString token;

  token = req.getUri();
  tokens = token.ft_split("?");
  envVec = setEnvCGI(tokens, req);
  argsVec.push_back(loc.getCgiPath());
  argsVec.push_back(tokens[0]);
  *env = vectorToCharArray(envVec);
  if (!*env)
    return (EXIT_FAILURE);
  *args = vectorToCharArray(argsVec);
  if (!*args) {
    freeCharArray(*env);
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

Response CGIHandler(Request &req, Client &client, Location &loc, Server &serv) {
  int pid;
  int status;
  int pipeFd[2];
  std::string method = req.getMethod();
  char **env;
  char **args;

  FtString uriToken = req.getUri();
  std::vector<std::string> uriParts = uriToken.ft_split("?");
  std::string scriptPath = uriParts.size() ? uriParts[0] : std::string();
  if (scriptPath.empty() || access(loc.getCgiPath().c_str(), X_OK) == -1 ||
      access(scriptPath.c_str(), X_OK) == -1)
    return Response(req.getVersion(), serv.getErrorPageByCode(BAD_REQUEST));
  if (getContext(&env, &args, loc, req))
    return Response(req.getVersion(),
                    serv.getErrorPageByCode(INTERNAL_SERVER_ERROR));
  if (method == "POST") {
    if (pipe(pipeFd))
      return Response(req.getVersion(),
                      serv.getErrorPageByCode(INTERNAL_SERVER_ERROR));
    write(pipeFd[1], req.getBody().c_str(), req.getBody().size());
    close(pipeFd[1]);
  }
  pid = fork();
  if (pid == -1)
    return Response(req.getVersion(),
                    serv.getErrorPageByCode(INTERNAL_SERVER_ERROR));
  if (pid == 0) {
    if (method == "POST")
      executeChild(pipeFd[0], client.getFd(), args, env);
    else
      executeChild(0, client.getFd(), args, env);
  }
  if (req.getMethod() == "POST")
    close(pipeFd[0]);
  waitpid(pid, &status, 0);
  if (WIFEXITED(status))
    return Response(req.getVersion(),
                   serv.getErrorPageByCode(INTERNAL_SERVER_ERROR));
  return Response();
  ;
}

void MethodExecutor::execute() {
  Location loc = this->getRequestLocation(this->_request, this->_server);
  if (loc.getCode() == PAGE_NOT_FOUND) {
    this->_response =
        Response(this->_request.getVersion(),
                 this->_server.getErrorPageByCode(PAGE_NOT_FOUND));
    return;
  }
  if (returnHandler(this->_response, loc, this->_request, this->_server) == 0)
    return;
  std::string fileName(loc.getRoot());
  fileName += this->_request.getUri();

  if (isCGI(this->_request, loc))
    this->_response =
        CGIHandler(this->_request, this->_client, loc, this->_server);
  else if (this->_method == "GET" &&
           std::find(loc.getAllowedMethods().begin(),
                     loc.getAllowedMethods().end(),
                     "GET") != loc.getAllowedMethods().end())
    this->_response =
        AHttpMethod::GET(fileName, loc, this->_request, this->_server);
  else if (this->_method == "POST")
    this->_response =
        AHttpMethod::POST(fileName, loc, this->_request, this->_server);
  else if (this->_method == "DELETE")
    this->_response =
        AHttpMethod::DELETE(fileName, this->_request, this->_server);
  else
    this->_response =
        Response(this->_request.getVersion(),
                 this->_server.getErrorPageByCode(METHOD_NOT_ALLOWED));
}

MethodExecutor::~MethodExecutor() {}