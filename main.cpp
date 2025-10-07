#include "Server.hpp"

int main() {
	Server	server;
	try {
		server.fillServerContent("main.cpp");
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}