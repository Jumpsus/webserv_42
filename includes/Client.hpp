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

        /*relate to internal server*/
        void                setSocket(int newsock);

        /*related to request and response*/
        bool            feed(std::string input, size_t len); // return true when request is complete
        void            setRequest(Request req);
        std::string     getResponse();
        bool            keepAlive();
        void            clearContent();
        void            buildResponse();
        void            fillReqBody();
        Request         req;
        Response        resp;

        /*relate to time*/
        const time_t&   getTime() const; //get time stamp
        void            updateTime(); //update time stamp

    private:
        int             _socket;
        Server          _serv;
        
        time_t          _time_stamp;

        std::string     _raw_request;
};

#endif