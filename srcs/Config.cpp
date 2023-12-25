#include "config/Config.hpp"

Config::Config(std::string const configPath)
{
    this->_path = configPath;
    this->readContent();
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
    std::string tempContent = this->_content;

    trimPrefixSuffixConfig(tempContent);

    while (tempContent.length() > 0)
    {
        std::string block = "";
        if (!getBlock(tempContent, block, "server"))
        {
            return false;
        }

        Server serv(block);
        serv.printServerInfo();
        this->_servers.push_back(serv);
        trimPrefixSuffixConfig(tempContent);
    }
    return true;
}

std::string Config::getContent()
{
    return (this->_content);
}