#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <iostream>
# include "config/Location.hpp"
# include "utils.hpp"
# include "Webserv.hpp"

# define DEFAULT_MAX_BODY_SIZE 1000000;

class Location;

class Server {
    public:
        Server();
        Server(std::string const serverConfig);
        Server(Server const &serv);
        Server &operator=(Server const &serv);
        ~Server();

        int                         getFd();
        void                        setFd(int sd);
        unsigned long               getHost() const;
        int                         getPort() const;
        std::string                 getServerName() const;
        std::map<int, std::string>  getErrorPage() const;
        std::vector<std::string>    getIndex() const;
        bool                        getAutoIndex() const;
        size_t                      getClientMaxBodySize() const;
        std::vector<Location>       getLocations() const;
        std::string                 getRoot() const;

        void                        setServer();
        void                        printServerInfo();
        void                        checkDupLocation(const Location& ori, const Location& check);

    private:
        int                             _fd;
        unsigned long                   _host;
        int                             _port;
        std::string                     _server_name;
        std::map<int, std::string>      _error_page;
        std::vector<std::string>        _index;
        bool                            _autoindex;
        size_t                          _client_max_body_size;
        std::vector<Location>           _locations;
        std::string                     _root;

        void                            initErrorPage();
        void                            parseServer(std::string serverConfig);
        bool                            setServerParameter(std::string param, std::vector<std::string> value);
};

#endif
