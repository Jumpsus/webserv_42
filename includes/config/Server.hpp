#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <vector>
# include <iostream>
# include "config/Location.hpp"
# include "utils.hpp"

# define DEFAULT_MAX_BODY_SIZE 1000000;

class Location;

class Server {
    public:
        // Server();
        Server(std::string const serverConfig);
        Server(Server const &serv);
        Server &operator=(Server const &serv);
        ~Server();

        void printServerInfo();
    private:
        std::string                     _host;
        int                             _port;
        std::string                     _server_name;
        std::map<int, std::string>      _error_page;
        std::vector<std::string>        _index;
        bool                            _autoindex;
        size_t                          _client_max_body_size;
        // std::vector<Location>           _locations;
        std::string                     _root;

        void                            parseServer(std::string serverConfig);
};

#endif