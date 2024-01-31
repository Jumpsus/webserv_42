#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# define MAX_CONNECTION 1000
# define CONNECTION_TIMEOUT 60
# define CLIENT_BUFFER 100000

# define RESET          "\x1B[0m"
# define RED            "\x1B[31m"
# define WHITE          "\x1B[37m"
# define YELLOW         "\x1B[33m"
# define BLUE           "\x1B[94m"
# define CYAN           "\x1B[36m"

# define MAGENTA        "\x1B[95m"

# include "Webserv.hpp"
# include "config/Server.hpp"
# include "Client.hpp"

class Server;
class Client;

/* https://www.ibm.com/docs/en/i/7.2?topic=designs-example-nonblocking-io-select */
class ServerManager {
    public:
        ServerManager(std::vector<Server> const servers);
        ServerManager(ServerManager const &serv_mng);
        ServerManager &operator=(ServerManager const &serv_mng);
        ~ServerManager();

        void    setServers();
        void    startServers();

    private:
        std::vector<Server>     _servers;
        int                     _max_fd;
        fd_set                  _server_fd; //subset of _read_fd
        fd_set                  _read_fd;
        fd_set                  _write_fd;
        std::map<int, Server>   _servers_map;
        std::map<int, Client>   _clients_map;

        void                    startlisten();
        void                    acceptConnection(int server_fd);
        void                    receiveRequest(int read_fd);
        void                    writeResponse(int write_fd);
        void                    closeConnection(const int i);
        void                    addSet(int fd, fd_set* set);
        void                    removeSet(int fd, fd_set* set);
        void                    checkTimeout();
        void                    writeCgi(int write_fd, CgiHandler& cgi);
        void                    readCgi(int read_fd, CgiHandler& cgi);
};

#endif