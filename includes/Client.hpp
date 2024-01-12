#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "config/Server.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Server;
class Request;
class Response;

class Client {
    public:
        Client();
        Client(Server const serv);
        Client(Client const &cli);
        Client &operator=(Client const &cli);
        ~Client();
        Request     req;
        Response    resp;

        void    setSocket(int fd);

    private:
        int                 _c_socket_fd;
        Server      _serv;
        //Request     _req;
        //Response    _resp;

        std::string _raw_request;
};

#endif