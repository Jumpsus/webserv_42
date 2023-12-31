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
        std::cout << "Set Server:" << it->getHost() << " Port:" << it->getPort() << " at fd: " << it->getFd() << std::endl;
    }
    return;
}

void    ServerManager::startServers()
{
    fd_set          current_read;
    fd_set          current_write;
    struct timeval  timeout;

    timeout.tv_sec  = 1 * 60 /2;
    timeout.tv_usec = 0;

    this->startlisten();

    do {
        memcpy(&current_read, &_read_fd, sizeof(_read_fd));
        memcpy(&current_write, &_write_fd, sizeof(_write_fd));

        int ready = select(_max_fd + 1, &current_read, &current_write, NULL, &timeout);

        std::cout << "ready " << ready << std::endl;
        if (ready < 0)
        {
            std::cerr << "select failed: " << errno << std::endl;
            break;
        }

        if (ready == 0)
        {
            std::cerr << "select timeout: " << errno << std::endl;
            break;
        }

        for (int i = 0; (i <= _max_fd); ++i)
        {
            if (FD_ISSET(i, &_server_fd))
            {
                std::cout << "accept "<< std::endl;
                acceptConnection(i);
            } else if (FD_ISSET(i, &current_read)) {
                receiveRequest(i);
                ready--;
            } else if (FD_ISSET(i, &current_write)) {
                // ready--;
            }
        }

    } while(1);

    std::cout << "graceful exit()" << std::endl;
}

void    ServerManager::startlisten()
{
    FD_ZERO(&_server_fd);
    FD_ZERO(&_read_fd);
    FD_ZERO(&_write_fd);

    for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        if (listen(it->getFd(), MAX_CONNECTION) < 0)
        {
            std::cerr << "listen fd " << it->getFd() << " error : " << std::endl;
            exit (-1);
        }

        if (fcntl(it->getFd(), F_SETFL, O_NONBLOCK) < 0)
        {
            std::cerr << "fcntl fd " << it->getFd() << " error : " << std::endl;
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
}

/* please mark server socket as non-bloacking before use accept() 
   if not accept will block other connection and make server misbehaviour */
void    ServerManager::acceptConnection(int server_fd)
{
    std::cout << "start accept " << std::endl;
    if (!_servers_map.count(server_fd))
    {
        std::cerr << "could not found server map for fd : " << server_fd << std::endl;
        exit(-1);
    }

    int new_fd = accept(server_fd, NULL, NULL);

    if (new_fd < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            std::cerr << "error accept:" << errno << " end server now" << std::endl;
        }
        return ;
    }

    if (fcntl(new_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl fd " << new_fd << " error : " << std::endl;
        close(new_fd);
        return ;
    }

    if (_clients_map.count(new_fd) > 0)
    {
        _clients_map.erase(new_fd);
    }

    _clients_map.insert(std::pair<int, Client>(new_fd, Client(_servers_map[server_fd])));

    addSet(new_fd, &_read_fd);

    std::cout << "got new fd: "<< new_fd << std::endl;
}

/* please mark read fd as non-bloacking before use recv() 
   if not recv will block other connection and make server misbehaviour */
void    ServerManager::receiveRequest(int read_fd)
{
    std::cout << "start recv " << std::endl;
    char buffer[CLIENT_BUFFER];

    /* After recv fd is set not ready for read until Client cancelled connection */
    int rc = recv(read_fd, buffer, sizeof(buffer), 0);

    if (rc < 0) {
        std::cerr << "[Crash] error recv:" << errno << "end server now" << std::endl;
        return ;
    }

    if (rc > 0) {
        /* TODO: feed request to client */
        /* TODO: parse request process client */
        std::string feed(buffer);
        std::cout << feed << std::endl;
        memset(buffer, 0, sizeof(buffer));
        return ;
    }

    /* Client send close connection */
    if (rc == 0) {
        std::cout << "Client Fd " << read_fd << " Closed connection!" << std::endl;
        /* TODO: close connection */
        removeSet(read_fd, &_read_fd);
        if (_clients_map.count(read_fd) > 0)
        {
            _clients_map.erase(read_fd);
        }
        close (read_fd);
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
    }

    _max_fd = temp_max;
}
