#include "Client.hpp"


Client::Client()
{
    _time_stamp = time(NULL);
}

Client::Client(Server const serv): _serv(serv)
{
    this->resp.setServer(serv);
    this->req.setMaxBodySize(serv.getClientMaxBodySize());
    _time_stamp = time(NULL);
}

Client::Client(Client const &cli): _serv(cli._serv)
{
    if (this != &cli)
    {
        _serv = cli._serv;
        this->req = cli.req;
        this->resp = cli.resp;
        _raw_request = cli._raw_request;
    }
}

Client &Client::operator=(Client const &cli)
{
    if (this != &cli)
    {
        _serv = cli._serv;
        this->req = cli.req;
        this->resp = cli.resp;
        _raw_request = cli._raw_request;
    }

    return (*this);
}

Client::~Client() {}

unsigned long Client::getServerHost()
{
    return this->_serv.getHost();
}

int     Client::getServerPort()
{
    return this->_serv.getPort();
}

void    Client::setServer(const Server& serv)
{
    this->_serv = serv;
}

void    Client::setSocket(int newsock)
{
    this->_socket = newsock;
}

bool    Client::feed(std::string input, size_t len)
{
    _raw_request = _raw_request + input.substr(0, len);

    Request r;

    if (!r.parseRequest(_raw_request))
    {
        // std::cout << "parse Request failed" << std::endl; 
        return false;
    }
    setRequest(r);
    return true;
}

void        Client::setRequest(Request req)
{
    this->req = req;
    this->resp.setRequest(req);
    return ;
}

std::string Client::getResponse()
{
    return (resp.getResponse());
}

bool        Client::keepAlive()
{
    return (req.keepAlive());
}

void        Client::clearContent()
{
    _raw_request = "";
    req.clear();
    resp.clear();
}

void        Client::buildResponse()
{
    resp.buildResponse();
}

const time_t& Client::getTime() const
{
    return _time_stamp;
}

void        Client::updateTime()
{
    _time_stamp = time(NULL);
}

void        Client::fillReqBody()
{
    if (req.getBody().length() == 0)
    {
        std::fstream file;(resp.cgi.getCgiPath().c_str());
        std::stringstream ss;

        ss << file.rdbuf();
        req.setBody(ss.str());
    }
}