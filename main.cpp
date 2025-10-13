#include "Context.hpp"
#include "Request.hpp"

int main(int ac, char **av) {
	Context	ctx;

	(void) ac;
	(void) av;
	if (ac != 2)
	{
		std::cerr << "Usage: ./webserv [ConfigFile]" << std::endl;
		return 1;
	}
	try {
		ctx.configFileParser(av[1]);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	std::string reqExample;
	reqExample += "GET / HTTP/1.1\r\n";
	reqExample += "Host: localhost\r\n";
	reqExample += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\r\n";
	reqExample += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
	reqExample += "Accept-Language: en-US,en;q=0.5\r\n";
	reqExample += "Accept-Encoding: gzip, deflate\r\n";
	reqExample += "Connection: keep-alive\r\n";
	reqExample += "Cache-Control: max-age=0\r\n";
	reqExample += "\r\n";

	const std::vector<Server> servers = ctx.getServers();
	Server serv = servers[0];
	Request req(serv);
	try {
		req.init(reqExample);
	} catch (std::exception &e) {
		
		std::cout << e.what() << std::endl;
	}

	FtString str("  aa   aab    c   ");

	std::vector<std::string> vec = str.ft_split(" ");
	for (size_t i = 0; i < vec.size(); i++) {
		std::cout << "'" << vec[i] << "'" << std::endl;
	}

	// req.displayRequest();

	return 0;
}