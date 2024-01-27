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

        void            setSocket(int fd);
        bool            feed(std::string input, size_t len); // return true when request is complete
        
        void            setRequest(Request req);
        std::string     getResponse();
        bool            keepAlive();
        void            clearContent();

        void            buildResponse();
        void            fillReqBody();
        
        Request             req;
        Response            resp;

        const struct timeval&   getTime() const; //get time stamp
        void                    updateTime(); //update time stamp

    private:
        int                 _socket_fd;
        Server              _serv;
        
        struct timeval      _time_stamp;

        std::string         _raw_request;
};

#endif