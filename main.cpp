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

    ac == 1 ? config_path = "configs/default.conf" : config_path = av[1];
    try 
    {
        Config conf(config_path);
        //conf.printConfigInfo();
        servers = conf.getServers();
        ServerManager server_manager(servers);
        server_manager.setServers();
        server_manager.startServers();
    }
    catch(std::string error) 
    {  
        std::cout << RED << "error: " << error << RESET << std::endl;
    }

    //for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    //{
    //    it->printServerInfo();
    //}
    // std::string content = c.getContent();

    // std::cout<< content <<std::endl;

    // std::string block;

    // int index = 0;

    // while (ft_is_white_space(content[index]))
    // {
    //     index++;
    // }

    // std::cout << "index = " << index <<std::endl;
    // block = findBlock(content.substr(index, content.length()), "location");
    
    // if (block.length() == 0){
    //     std::cout << "get block fail" << std::endl;
    // }

    // std::cout << "block = " << block << std::endl;
    // std::cout << "content = " << content << content.length() << std::endl;
}