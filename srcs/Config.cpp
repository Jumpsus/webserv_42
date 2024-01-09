#include "config/Config.hpp"

Config::Config(std::string const configPath)
{
    std::cout << "start Config()" << std::endl;
    this->_path = configPath;

    std::cout << "read Config()" << std::endl;
    this->readContent();

    std::cout << "parse Config()" << std::endl;
    this->parseConfig();

    if (this->_servers.size() == 0)
        throw(std::string("can't build server"));
}

Config::Config(Config const &configFile)
{
    if (this != &configFile)
    {
        this->_path = configFile._path;
        this->_content = configFile._content;
    }
}

Config::~Config() {}

Config &Config::operator=(Config const &configFile)
{
    if (this != &configFile)
    {
        this->_path = configFile._path;
        this->_content = configFile._content;
    }
    return (*this);
}

bool    Config::validateConfig()
{
    std::ifstream config_file(_path.c_str());
	if (!config_file || !config_file.is_open())
		return false;

    return true;
}

void    Config::readContent()
{
    std::string temp;
    bool        comment = false;

    std::ifstream config_file(_path.c_str());
	if (!config_file || !config_file.is_open())
    {
		return;
    }

	std::stringstream stream_binding;
	stream_binding << config_file.rdbuf();
	temp = stream_binding.str();

    config_file.close();

    /* remove comment */
    for (int i = 0; i < temp.length(); i++)
    {
        if (!comment)
        {
            if (temp[i] == '#')
            {
                comment = true;
            } else {
                _content = _content + temp[i];
            }
        } else {
            if (temp[i] == '\n')
            {
                comment = false;
            }
        }
    }
}

bool    Config::parseConfig()
{
    std::string content = this->_content;
    size_t index = 0;

    while (index < content.length())
    {
        while (ft_is_white_space(content[index]))
        {
            index++;
        }

        std::string content = this->_content.substr(index, this->_content.length());
        std::string block = findBlock(content ,"server ", true);
        index += shiftBlock(content, "server");
        if (block.length() == 0)
        {
            return false;
        }
        Server serv(block);
        if (this->_servers.size() > 1)
        {
            for (size_t si = 0; si < this->_servers.size(); si++)
                checkDupServer(this->_servers[si], serv);
        }
        this->_servers.push_back(serv);
        
        index += block.length();
    }
    return true;
}

std::string Config::getContent()
{
    return (this->_content);
}

void Config::printConfigInfo()
{
    std::vector<Server>::iterator it;

    for (it = this->_servers.begin(); it != _servers.end(); it++)
    {
        (*it).printServerInfo();
    }
}

void Config::checkDupServer(const Server& ori, const Server& check)
{
    if (ori.getHost() == check.getHost() && ori.getPort() == check.getPort() &&
        ori.getServerName() == check.getServerName())
        throw (std::string("found duplicate servers")); 
}