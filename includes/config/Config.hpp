#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include "config/Server.hpp"
# include "utils.hpp"

class Server;

class Config {
    public:
        Config(std::string const configPath);
        Config(Config const &conf);
        Config &operator=(Config const &conf);
        ~Config();

        bool            validateConfig();
        std::string     getContent();
        bool            validateContent();
        void            printConfigInfo();
        void            checkDupServer(const Server& ori, const Server& check);

    private:
        std::string         _path;
        std::string         _content;
        std::vector<Server> _servers;

        void            readContent();
        bool            parseConfig();
};

#endif