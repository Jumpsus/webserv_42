#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# define MAX_CONNECTION 1000
# define CONNECTION_TIMEOUT 30
# define CLIENT_BUFFER 1000

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
        void                    receiveRequest(int read_fd, Client c);
        void                    closeConnection(const int i);
        void                    addSet(int fd, fd_set* set);
        void                    removeSet(int fd, fd_set* set);  
};

#endif