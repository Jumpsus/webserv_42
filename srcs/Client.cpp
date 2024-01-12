#include "Client.hpp"


Client::Client() {}

Client::Client(Server const serv): _serv(serv) {}

Client::Client(Client const &cli): _serv(cli._serv)
{
    if (this != &cli)
    {
        _serv = cli._serv;
        req = cli.req;
        resp = cli.resp;
        _raw_request = cli._raw_request;
    }
}

Client &Client::operator=(Client const &cli)
{
    if (this != &cli)
    {
        _serv = cli._serv;
        req = cli.req;
        resp = cli.resp;
        _raw_request = cli._raw_request;
    }

    return (*this);
}

Client::~Client() {}

void    Client::setSocket(int fd) {
    _c_socket_fd = fd;
}

