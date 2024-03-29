#include "utils.hpp"
#include "Request.hpp"
#include "config/Config.hpp"
#include "ServerManager.hpp"
#include <iostream>
#include <arpa/inet.h>

int main(int ac, char **av)
{
    std::vector<Server> servers;
    std::string         config_path;

    if (ac > 2) {
        std::cout << YELLOW << "Webserv don't support more than 1 argument" << YELLOW << std::endl;
        return 1;
    }
    ac == 1 ? config_path = "configs/default.conf" : config_path = av[1];
    try 
    {
        Config conf(config_path);
        servers = conf.getServers();
        ServerManager server_manager(servers);
        server_manager.setServers();
        server_manager.startServers();
    }
    catch(std::string error) 
    {  
        std::cout << RED << "error: " << error << RESET << std::endl;
        return (1);
    }

    //conf.printConfigInfo();
    
}