#include "config/Server.hpp"

Server::Server(std::string const serverConfig)
{

    /* set default value */
    _host = "";
    _port = 80;
    _server_name = "";
    _client_max_body_size = DEFAULT_MAX_BODY_SIZE; //1MB
    _autoindex = true;
    _root = "";
}

void    Server::parseServer(std::string serverConfig)
{
    if (!removeBracket(serverConfig, "server"))
    {
        throw ("Invalid syntax");
    }
}

Server::Server(Server const &serv)
{
    if (this != &serv) {
        _host = serv._host;
        _port = serv._port;
        _server_name = serv._server_name;
        _error_page = serv._error_page;
        _index = serv._index;
        _autoindex = serv._autoindex;
        _client_max_body_size = serv._client_max_body_size;
        // _locations = serv._locations;
        _root = serv._root;
    }
}

Server  &Server::operator=(Server const &serv)
{
    if (this != &serv) {
        _host = serv._host;
        _port = serv._port;
        _server_name = serv._server_name;
        _error_page = serv._error_page;
        _index = serv._index;
        _autoindex = serv._autoindex;
        _client_max_body_size = serv._client_max_body_size;
        // _locations = serv._locations;
        _root = serv._root;
    }
    return (*this);
}

Server::~Server()
{
    _error_page.clear();
    _index.clear();
    // _locations.clear();
}

void    Server::printServerInfo()
{
    std::cout << "host: " << this->_host << std::endl;
    std::cout << "port: " << this->_port << std::endl;
    std::cout << "server_name: " << this->_server_name << std::endl;

    std::cout << "error_page: " << std::endl;
    std::map<int, std::string>::iterator itErrorPage;
    for (itErrorPage = this->_error_page.begin(); itErrorPage != this->_error_page.end(); itErrorPage++){
        std::cout << "- first value :" << itErrorPage->first << " ,second value: " << itErrorPage->second << std::endl;
    }

    std::cout << "index: " << std::endl;
    std::vector<std::string>::iterator itIndex;
    for (itIndex = this->_index.begin(); itIndex < this->_index.end(); itIndex++){
        std::cout << "- " << *itIndex << std::endl;
    }

    std::cout << "autoindex: " << this->_autoindex << std::endl;
    std::cout << "client_max_body_size: " << this->_client_max_body_size << std::endl;
    std::cout << "root: " << this->_root << std::endl;
}
