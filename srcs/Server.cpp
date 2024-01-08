#include "config/Server.hpp"

Server::Server() {
    // shouldn't call this function;
}

Server::Server(std::string const serverConfig)
{
    std::cout << "start Server()" << std::endl;
    /* set default value */
    _fd = 0;
    _host = 0;
    _port = 80;
    _server_name = "";
    _client_max_body_size = DEFAULT_MAX_BODY_SIZE; //1MB
    _autoindex = true;
    _root = "";

    initErrorPage();
    parseServer(serverConfig);
}

Server::Server(Server const &serv)
{
    if (this != &serv) {
        _fd = serv._fd;
        _host = serv._host;
        _port = serv._port;
        _server_name = serv._server_name;
        _error_page = serv._error_page;
        _index = serv._index;
        _autoindex = serv._autoindex;
        _client_max_body_size = serv._client_max_body_size;
        _locations = serv._locations;
        _root = serv._root;
    }
}

void    Server::initErrorPage()
{
    _error_page[400] = "assets/default-error/400.html";
    _error_page[403] = "assets/default-error/403.html";
    _error_page[404] = "assets/default-error/404.html";
    _error_page[405] = "assets/default-error/405.html";
    _error_page[410] = "assets/default-error/410.html";
    _error_page[413] = "assets/default-error/413.html";
    _error_page[500] = "assets/default-error/500.html";
}

void    Server::parseServer(std::string serverConfig)
{
    int                         index = 0;
    bool                        inBracket = false;
    std::string                 word;
    std::string                 currentParameter;
    std::vector<std::string>    storeValue;

    while (index < serverConfig.length())
    {
        if (ft_is_white_space(serverConfig[index]))
        {
            index++;
            continue;
        }

        word = findNextWord(serverConfig.substr(index, serverConfig.length()));
        // std::cout << "currentParameter " << currentParameter << " word = " << word << " index: " << index << std::endl;
        if (currentParameter == "")
        {
            if (word == "server")
            {
                // std::cout << "in server" << std::endl;
                if (inBracket)
                {
                    std::string errorMessage = "Invalid syntax found server inside server" ;
                    storeValue.clear();
                    std::cout << errorMessage << std::endl;
                    throw(errorMessage);
                }
                currentParameter = word;
                index += word.length();
            } else if (word == "location") {
                std::string block = findBlock(serverConfig.substr(index, serverConfig.length()) ,"location");
                if (block.length() == 0)
                {
                    std::string errorMessage = "Invalid syntax found on location Block" ;
                    storeValue.clear();
                    throw(errorMessage);
                }

                Location loc(block, *this);
                // loc.printLocationInfo();
                this->_locations.push_back(loc);
                index += block.length();
            } else if (word == "}") {
                if (!inBracket)
                {
                    std::string errorMessage = "Invalid syntax found no scope before }" ;
                    storeValue.clear();
                    throw(errorMessage);
                }
                inBracket = false;
                index += word.length();
            } else {
                // throw(word);
                // storeValue.push_back(word);
                // index += word.length();
                currentParameter = word;
                index += word.length();
            }
        } else {
            if (word == "{") { 
                if (currentParameter == "server") {
                    inBracket = true;
                    storeValue.clear();
                    currentParameter = "";
                } else {
                    std::string errorMessage = "Invalid syntax: Found \'{\' in wrong place after ", currentParameter ;
                    throw(errorMessage);
                }
                    index += word.length();
            } else if (word == ";") {
                if (!setServerParameter(currentParameter, storeValue))
                {
                    std::string errorMessage = "Cannot set value " + word;
                    std::vector<std::string>::iterator    it;

                    for (it = storeValue.begin(); it != storeValue.end(); it++)
                    {
                        errorMessage = errorMessage + " " + *it;
                    }

                    errorMessage = errorMessage + " in Parameter :" + currentParameter;
                    throw(errorMessage);
                }
                currentParameter = "";
                storeValue.clear();
                index += word.length();
            } else {
                storeValue.push_back(word);
                index += word.length();
            }
        }
    }
    storeValue.clear();
    return ;
}

bool Server::setServerParameter(std::string param, std::vector<std::string> value)
{
    if (param == "host")
    {
        if (value.size() != 1)
        {
            return false;
        }

        if (value[0] == "localhost")
        {
            value[0] = "127.0.0.1";
        }

        if (!ft_is_valid_host(value[0]))
        {
            return false;
        }

        this->_host = ft_pton(value[0]);
        
    } else if (param == "listen") {
        if (value.size() != 1 || !ft_isdigit(value[0]))
        {
            return false;
        }
        this->_port = ft_stoi(value[0]);
        
    } else if (param == "server_name") {
        if (value.size() != 1)
        {
            return false;
        }
        this->_server_name = value[0];

    } else if (param == "error_page") {
        if (value.size() != 2 || !ft_isdigit(value[0]))
        {
            return false;
        }

        this->_error_page[ft_stoi(value[0])] = value[1];
        
    } else if (param == "index") {
         if (value.size() < 1)
        {
            return false;
        }

        this->_index = value;
        
    } else if (param == "autoindex") {
        if (value.size() != 1)
        {
            return false;
        }

        if (value[0] == "on")
        {
            this->_autoindex = true;
        } else if (value[0] == "off") {
            this->_autoindex = false;
        } else {
            return false;
        }

        return true;

    } else if (param == "client_max_body_size") {
        if (value.size() != 1 || !ft_isdigit(value[0]))
        {
            return false;
        }

        this->_client_max_body_size = ft_stoi(value[0]);
    } else if (param == "root") {
        if (value.size() != 1)
        {
            return false;
        }
        this->_root = value[0];
        
    } else {
       return false;
    }
    return true;
}

Server  &Server::operator=(Server const &serv)
{
    if (this != &serv) {
        _fd = serv._fd;
        _host = serv._host;
        _port = serv._port;
        _server_name = serv._server_name;
        _error_page = serv._error_page;
        _index = serv._index;
        _autoindex = serv._autoindex;
        _client_max_body_size = serv._client_max_body_size;
        _locations = serv._locations;
        _root = serv._root;
    }
    return (*this);
}

Server::~Server()
{
    _error_page.clear();
    _index.clear();
    _locations.clear();
}

int                         Server::getFd()
{
    return this->_fd;
}

void                        Server::setFd(int sd)
{
    this->_fd = sd;
    return ;
}

unsigned long               Server::getHost() const
{
    return this->_host;
}

int                         Server::getPort() const
{
    return this->_port;
}

std::string                 Server::getServerName() const
{
    return this->_server_name;
}

std::map<int, std::string>  Server::getErrorPage() const
{
    return this->_error_page;
}

std::vector<std::string>    Server::getIndex() const
{
    return this->_index;
}

bool                        Server::getAutoIndex() const
{
    return this->_autoindex;
}

size_t                      Server::getClientMaxBodySize() const
{
    return this->_client_max_body_size;
}

std::string                 Server::getRoot() const
{
    return this->_root;
}

void                        Server::setServer()
{
    struct sockaddr_in addr;

    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0)
    {
        std::cerr << "init socket error " << std::endl;
        exit(-1);
    }

    int option = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option))) {
        std::cerr << "set sock opt fail :" << errno << std::endl;
        close(_fd);
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = this->_host;
    addr.sin_port = htons(this->_port);

    if (bind(_fd, (struct sockaddr*) &addr, sizeof(addr))) {
        std::cerr << "bind error: " << errno << std::endl;
        close(_fd);
        exit(-1);
    }

    return ;
}

void    Server::printServerInfo()
{
    std::cout << "====== SERVER ======" << std::endl;
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

    std::vector<Location>::iterator itLocation;
    for (itLocation = this->_locations.begin(); itLocation != _locations.end(); itLocation++)
    {
        (*itLocation).printLocationInfo();
    }
}
