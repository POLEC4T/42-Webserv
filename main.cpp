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



		std::vector<Server> _servers = ctx.getServers();
		for (size_t i = 0; i < _servers.size(); ++i) {
			std::cout << "Server " << i + 1 << ":\n";
			std::cout << "  Host: " << _servers[i].getHost() << "\n";
			std::cout << "  Port: " << _servers[i].getPort() << "\n";
			std::cout << "  Client Max Body Size: " << _servers[i].getClientMaxBodySize() << "\n";
			std::cout << "  Names: ";
			const std::vector<std::string>& names = _servers[i].getNames();
			for (size_t j = 0; j < names.size(); ++j) {
				std::cout << names[j];
				if (j + 1 < names.size()) std::cout << ", ";
			}
			std::cout << "\n";
			
			// Print locations
			std::map<std::string, Location>& locations = _servers[i].getLocations();
			if (!locations.empty()) {
				std::cout << "  Locations:\n";
				for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); ++it) {
					std::cout << "    Location: " << it->first << "\n";
					std::cout << "      Root: " << it->second.getRoot() << "\n";
					std::cout << "      Auto Index: " << (it->second.getAutoIndex() ? "on" : "off") << "\n";
					std::cout << "      Client Max Body Size: " << it->second.getClientMaxBodySize() << "\n";
					
					// Print index files
					const std::vector<std::string>& indices = it->second.getIndex();
					if (!indices.empty()) {
						std::cout << "      Index: ";
						for (size_t k = 0; k < indices.size(); ++k) {
							std::cout << indices[k];
							if (k + 1 < indices.size()) std::cout << ", ";
						}
						std::cout << "\n";
					}
					
					const std::vector<std::string>& method = it->second.getAllowedMethods();
					if (!method.empty()) {
						std::cout << "      Allowed Methods: ";
						for (size_t k = 0; k < method.size(); ++k) {
							std::cout << method[k];
							if (k + 1 < method.size()) std::cout << ", ";
						}
						std::cout << "\n";
					}
					
					if (!it->second.getUploadPath().empty())
						std::cout << "      Upload Path: " << it->second.getUploadPath() << "\n";
					if (!it->second.getReturn().empty())
						std::cout << "      Return: " << it->second.getReturn() << "\n";
					if (!it->second.getCgiExtension().empty())
						std::cout << "      CGI Extension: " << it->second.getCgiExtension() << "\n";
					if (!it->second.getCgiPath().empty())
						std::cout << "      CGI Path: " << it->second.getCgiPath() << "\n";
				}
			}
		std::cout << "\n";
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}