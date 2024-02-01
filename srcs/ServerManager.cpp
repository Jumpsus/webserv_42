#include "ServerManager.hpp"

ServerManager::ServerManager(std::vector<Server> const servers)
{
    this->_servers = servers;
    this->_max_fd = 0;
    return ;
}

ServerManager::ServerManager(ServerManager const &serv_mng)
{
    if (this != &serv_mng)
    {
        this->_servers = serv_mng._servers;
        this->_max_fd = serv_mng._max_fd;
        this->_server_fd = serv_mng._server_fd;
        this->_read_fd = serv_mng._read_fd;
        this->_write_fd = serv_mng._write_fd;
    }
    return ;
}

ServerManager &ServerManager::operator=(ServerManager const &serv_mng)
{
    if (this != &serv_mng)
    {
        this->_servers = serv_mng._servers;
        this->_max_fd = serv_mng._max_fd;
        this->_server_fd = serv_mng._server_fd;
        this->_read_fd = serv_mng._read_fd;
        this->_write_fd = serv_mng._write_fd;
    }
    return (*this);
}

ServerManager::~ServerManager() {}

void    ServerManager::setServers()
{
    std::cout << std::endl << YELLOW << "Initializing Servers" << RESET << std::endl;
    for (std::vector<Server>::iterator it = this->_servers.begin();
         it != this->_servers.end(); it ++)
    {
        bool    duplicate = false;

        for (std::vector<Server>::iterator it2 = this->_servers.begin();
            it2 != it; it2++)
        {
            if (it2->getPort() == it->getPort() &&
                it2->getHost() == it->getHost())
            {
                it2->setFd(it->getFd());
                duplicate = true;
                std::cout << "duplicated" << std::endl;
            }
        }

        if (!duplicate)
        {
            it->setServer();
        }
        std::cout << YELLOW << "Set Server:" << it->getHost() << " Port:" << it->getPort() << " at fd: " << 
                                it->getFd() << RESET << std::endl;
    }
    return;
}

void    ServerManager::startServers()
{
    fd_set          current_read;
    fd_set          current_write;
    struct timeval  timeout;

    timeout.tv_sec  = 20 * 60 /2;
    timeout.tv_usec = 0;

    this->startlisten();

    do {
        memcpy(&current_read, &_read_fd, sizeof(_read_fd));
        memcpy(&current_write, &_write_fd, sizeof(_write_fd));

        int ready = select(_max_fd + 1, &current_read, &current_write, NULL, &timeout);
        if (ready < 0)
        {
            std::cerr << RED << "select failed: " << errno << RESET << std::endl;
            break;
        }

        if (ready == 0)
        {
            std::cerr << YELLOW << "select timeout: " << errno << RESET << std::endl;
            break;
        }

        for (int i = 0; (i <= _max_fd); ++i)
        {            
            if (FD_ISSET(i, &_server_fd))
            {
                acceptConnection(i);
            } else if (FD_ISSET(i, &current_read)) {
                receiveRequest(i);
                ready--;
            } else if (FD_ISSET(i, &current_write)) {
                writeResponse(i);
                ready--;
            }
        }
        checkTimeout();
    } while(1);

    std::cout << YELLOW << "graceful exit()" << RESET << std::endl;
}

void    ServerManager::startlisten()
{
    FD_ZERO(&_server_fd);
    FD_ZERO(&_read_fd);
    FD_ZERO(&_write_fd);

    for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        /*set the listen back log*/
        if (listen(it->getFd(), MAX_CONNECTION) < 0)
        {
            std::cerr << RED << "listen fd " << it->getFd() << " error : " <<  RESET << std::endl;
            exit (-1);
        }

        if (fcntl(it->getFd(), F_SETFL, O_NONBLOCK) < 0) {
            std::cerr << RED << "fcntl fd " << it->getFd() << " error : " << RESET << std::endl;
            exit (-1);
        }
        
        if (!_servers_map.count(it->getFd()))
        {
            _servers_map.insert(std::pair<int, Server>(it->getFd(), *it));
        }

        /* server fd is subset of read fd */
        this->addSet(it->getFd(), &_server_fd);
        this->addSet(it->getFd(), &_read_fd);
    }
    _max_fd = _servers.back().getFd();
}

/* please mark server socket as non-bloacking before use accept() 
   if not accept will block other connection and make server misbehaviour */
void    ServerManager::acceptConnection(int server_fd)
{
    struct sockaddr_in  client_addr;
    socklen_t           client_addr_len = sizeof(client_addr);
    int                 client_fd;
    Client              new_client(_servers_map[server_fd]);
    char                buf[INET_ADDRSTRLEN];

    if (!_servers_map.count(server_fd))
    {
        std::cerr << RED << "could not found server map for fd : " << server_fd << RESET << std::endl;
        exit(-1);
    }
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            std::cerr << RED << "error accept:" << errno << " end server now" << RESET << std::endl;
        }
        return ;
    }
    std::cout << YELLOW << "New connection from " << inet_ntop(AF_INET, &client_addr, buf, INET_ADDRSTRLEN) <<
                            ", Assigned socket " << client_fd << RESET << std::endl;
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl fd " << client_fd << " error : " << std::endl;
        removeSet(client_fd, &_read_fd);
        close(client_fd);
        return ;
    }
    new_client.setSocket(client_fd);
    if (_clients_map.count(client_fd) > 0)
    {
        _clients_map.erase(client_fd);
    }

    _clients_map.insert(std::pair<int, Client>(client_fd, new_client));

    addSet(client_fd, &_read_fd);
}

/* please mark read fd as non-bloacking before use recv() 
   if not recv will block other connection and make server misbehaviour */

void    ServerManager::receiveRequest(int read_fd)
{
    char buffer[CLIENT_BUFFER];

    /* After recv fd is set not ready for read until Client cancelled connection */
    int rc = recv(read_fd, buffer, sizeof(buffer), 0);

    if (rc < 0) {
        std::cerr << RED << "[Crash] error recv: " << errno << " at fd " << read_fd << " end server now" << RESET << std::endl;
        closeConnection(read_fd);
        return ;
    }

    if (rc > 0) {
        std::string req;
        req.assign(buffer, buffer + rc);

        _clients_map[read_fd].updateTime();
        if (_clients_map[read_fd].feed(req, rc))
        {
            std::cout << CYAN << "Request received from Socket " << read_fd << " Method = " << 
                                    _clients_map[read_fd].req.getMethod() << " URI= " << _clients_map[read_fd].req.getPath() << 
                                    RESET << std::endl;
            _clients_map[read_fd].req.printRequest();
            _clients_map[read_fd].buildResponse();
            removeSet(read_fd, &_read_fd);
            addSet(read_fd, &_write_fd);
            if (_clients_map[read_fd].resp.getCgiStatus() == true) {
                writeCgi(read_fd, _clients_map[read_fd].resp.cgi);
                return ;
            }
        }
        ft_memset(buffer, 0, sizeof(buffer));
        return ;
    }

    /* Client send close connection */
    if (rc == 0) {
        std::cout << CYAN << "Client Fd " << read_fd << " Closed connection!" << RESET << std::endl;
        closeConnection(read_fd);
    }
}

void    ServerManager::writeResponse(int write_fd)
{
    //std::cout << "write response, write_fd = " << write_fd << std::endl;
    if (_clients_map[write_fd].resp.getCgiStatus() == true)
    {
        readCgi(write_fd, _clients_map[write_fd].resp.cgi);
        return ;
    }
    std::string resp = _clients_map[write_fd].getResponse();

    //std::cout << "Response = " << resp << std::endl;

    /* After recv fd is set not ready for read until Client cancelled connection */
    int rc = send(write_fd, resp.c_str(), resp.length(), 0);
    
    if (rc < 0) {
        std::cerr << RED << "[Crash] error send: " << errno << " at fd " << write_fd << " end server now" << 
                    RESET << std::endl;
        closeConnection(write_fd);
        return ;
    }
    
    if (_clients_map[write_fd].req.keepAlive())
    {
        std::cout << CYAN << "Response send to Socket " << write_fd << " Stats = " << 
                                    _clients_map[write_fd].resp.getStatus() << RESET << std::endl; 
        _clients_map[write_fd].clearContent();
        removeSet(write_fd, &_write_fd);
        addSet(write_fd, &_read_fd);
    } else {
        closeConnection(write_fd);
    }
    _clients_map[write_fd].updateTime();
}

void    ServerManager::writeCgi(int write_fd, CgiHandler& cgi)
{
    std::string body = _clients_map[write_fd].req.getBody();
    int         body_sent = body.length();

    std::cout << CYAN << "Write cgi, body = " << body << " fd = " << write_fd << RESET << std::endl; 
    body_sent = write(cgi.pipe_in[1], body.c_str(), body.length());
    
    if (body_sent < 0) {
        std::cerr << RED << "Fail to send body to cgi" << RESET << std::endl;
        close(cgi.pipe_in[1]);
        close(cgi.pipe_out[1]);
        _clients_map[write_fd].resp.setError(500);
    }
    else {
        std::cout << CYAN << "write success" << RESET << std::endl;
        close(cgi.pipe_in[1]);
        close(cgi.pipe_out[1]);
        _clients_map[write_fd].updateTime();
        body = body.substr(body_sent);
    }
}

void    ServerManager::readCgi(int read_fd, CgiHandler& cgi)
{
    char    pipe_out[MESSAGE_BUFFER * 2];
    int     byte_read = read(cgi.pipe_out[0], pipe_out, MESSAGE_BUFFER * 2);

    if (byte_read > 0) {
        _clients_map[read_fd].updateTime();

        std::string cgi_body = _clients_map[read_fd].resp.getBody();
        cgi_body.append(pipe_out, byte_read);
        _clients_map[read_fd].resp.setBody(cgi_body);
        ft_memset(pipe_out, 0, sizeof(pipe_out));
    }
    else if (byte_read < 0) {
        std::cerr << RED << "Fail to read cgi response" << RESET << std::endl;
        close(cgi.pipe_in[0]);
        close(cgi.pipe_out[0]);
        _clients_map[read_fd].resp.switchCgiOff();
        _clients_map[read_fd].resp.setError(500);
    }
    else {
        close(cgi.pipe_in[0]);
        close(cgi.pipe_out[0]);

        /*wait until cgi's child process is done*/
        int cgi_status;
        waitpid(cgi.getCgiPid(), &cgi_status, 0);
        if (WIFEXITED(cgi_status))
        {
            {
                if (WEXITSTATUS(cgi_status) != 0) {
                    std::cout << RED << "Cgi fail to execute" << RESET << std::endl;
                    _clients_map[read_fd].resp.setError(500);
                }
            }
        }
        _clients_map[read_fd].resp.switchCgiOff();
        _clients_map[read_fd].resp.editResponseToCgi();
    }
}

void    ServerManager::addSet(int fd, fd_set* set)
{
    FD_SET(fd, set);
    if (fd > _max_fd)
    {
       _max_fd = fd;
    }
}

void    ServerManager::removeSet(int fd, fd_set* set)
{
    FD_CLR(fd, set);

    int temp_max = 0;

    if (fd == _max_fd)
    {
        for (int i = 0; i < _max_fd; i++)
        {
            if (FD_ISSET(i, &_write_fd))
            {
                temp_max = i;
            }

            if (FD_ISSET(i, &_read_fd))
            {
                temp_max = i;
            }
        }
        _max_fd = temp_max;
    }
}

void    ServerManager::closeConnection(int fd)
{
    if (FD_ISSET(fd, &_server_fd))
    {
        removeSet(fd, &_server_fd);
    }

    if (FD_ISSET(fd, &_read_fd))
    {
        removeSet(fd, &_read_fd);
    }

    if (FD_ISSET(fd, &_write_fd))
    {
        removeSet(fd, &_write_fd);
    }

    close(fd);
    _clients_map.erase(fd);
}

void    ServerManager::checkTimeout()
{
    for (std::map<int, Client>::iterator cit = _clients_map.begin(); cit != _clients_map.end(); cit++)
    {
        if (time(NULL) - cit->second.getTime() > CONNECTION_TIMEOUT)
        {
            std::cout << YELLOW << "Client " << cit->first << " Timeout, closing connection" << RESET << std::endl;
            closeConnection(cit->first);
            return ;
        }   
    }
}