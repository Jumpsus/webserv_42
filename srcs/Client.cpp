#include "Client.hpp"


Client::Client() {}

Client::Client(Server const serv): _serv(serv)
{
    _resp.setServer(serv);
    _req.setMaxBodySize(serv.getClientMaxBodySize());
    gettimeofday(&_time_stamp, NULL);
}

Client::Client(Client const &cli): _serv(cli._serv)
{
    if (this != &cli)
    {
        _serv = cli._serv;
        _req = cli._req;
        _resp = cli._resp;
        _raw_request = cli._raw_request;
    }
}

Client &Client::operator=(Client const &cli)
{
    if (this != &cli)
    {
        _serv = cli._serv;
        _req = cli._req;
        _resp = cli._resp;
        _raw_request = cli._raw_request;
    }

    return (*this);
}

Client::~Client() {}

void    Client::setSocket(int fd) {
    _socket_fd = fd;
}

bool    Client::feed(std::string input, size_t len)
{
    _raw_request = _raw_request + input.substr(0, len);

    std::cout << "input = " << input << std::endl;
    std::cout << "len = " << input.length() << std::endl;
    Request r;

    if (!r.parseRequest(_raw_request))
    {
        std::cout << "parse Request failed" << std::endl; 
        return false;
    }

    std::cout << "error : " << r.getError() << std::endl;
    if (!r.getError())
    {
        std::cout << "set Error to 400 " << std::endl;
        r.setError(400);
    }

    setRequest(r);
    return true;
}

void        Client::setRequest(Request req)
{
    _req = req;
    _resp.setRequest(req);
    return ;
}

std::string Client::getResponse()
{
    return (_resp.getResponse());
}

void        Client::buildResponse()
{
    _resp.buildResponse();
}

const struct timeval&   Client::getTime() const
{
    return _time_stamp;
}

void        Client::updateTime()
{
    gettimeofday(&_time_stamp, NULL);
}

