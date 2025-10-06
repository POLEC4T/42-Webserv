#include "Server.hpp"
#include "ConfigFileParser.hpp"

int main(int ac, char **av) {
    ConfigFileParser    parser;

    if (ac != 2)
    {
        std::cerr << "Usage: ./webserv [ConfigFile]" << std::endl;
        return 1;
    }
    try {
    parser.configFileParser(av[1]);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}