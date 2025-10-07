#include "Context.hpp"
#include "Request.hpp"

int main(int ac, char **av) {
	Context	ctx;

	(void) ac;
	(void) av;
	// if (ac != 2)
	// {
	// 	std::cerr << "Usage: ./webserv [ConfigFile]" << std::endl;
	// 	return 1;
	// }
	// try {
	// 	ctx.configFileParser(av[1]);
	// }
	// catch (std::exception& e) {
	// 	std::cerr << e.what() << std::endl;
	// }
	std::string reqExample;
	reqExample += "GET /index.html HTTP/1.1\r\n";
	reqExample += "Host: www.example.com\r\n";
	reqExample += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\r\n";
	reqExample += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
	reqExample += "Accept-Language: en-US,en;q=0.5\r\n";
	reqExample += "Accept-Encoding: gzip, deflate\r\n";
	reqExample += "Connection: keep-alive\r\n";
	reqExample += "Cache-Control: max-age=0\r\n";
	reqExample += "\r\n";

	Request req(reqExample);


	// std::string test;
	// test += "11111:123\r\n"; // 0
	// test += "22222:456\r\n"; // 7
	// test += "33333:789\r\n"; // 14
	// test += "\r\n";
	// Request req(test);
	return 0;
}