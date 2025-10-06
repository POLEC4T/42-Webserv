#include "Context.hpp"

int main(int ac, char **av) {
	Context	ctx;

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
}