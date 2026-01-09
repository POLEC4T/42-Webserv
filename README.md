This project has been created as part of the 42 curriculum by [mniemaz](https://github.com/POLEC4T), [faoriol](https://github.com/faoriol) and [dmazari](https://github.com/mazakov-d)
# Webserv
An HTTP server built in C++ 98.
# Features
 - GET, POST and DELETE methods
 - HTTP redirection
 - Builtin CGIs
 - cookies and session management
# How to run
 - Clone the project
 - navigate to the project folder
 - run the command
   ```bash
   make
   ```
 - run the command (config file is optional, default is `./config/default.conf`)
   ```bash
   ./webserv [path_to_config_file]
   ```
 - Open your browser and navigate to `http://localhost:[port]` where `[port]` is the port specified in your config file.