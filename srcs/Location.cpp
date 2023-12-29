#include "config/Location.hpp"

Location::Location(std::string const locationConfig ,Server const &serv)
{
    this->_root = serv.getRoot();
    this->_index = serv.getIndex();
    this->_autoindex = serv.getAutoIndex();
    this->_client_max_body_size = serv.getClientMaxBodySize();

    initAllowMethods(true);
    parseLocation(locationConfig);

    return ;
}

Location::Location(Location const &loc)
{
    if (this != &loc)
    {
        _path = loc._path;
        _root = loc._root;
        _autoindex = loc._autoindex;
        _index = loc._index;
        _alias = loc._alias;
        _allow_methods = loc._allow_methods;
        _return = loc._return;
        _cgi_ext = loc._cgi_ext;
        _cgi_path = loc._cgi_path;
        _client_max_body_size = loc._client_max_body_size;
    }
    return ;
}

Location &Location::operator=(Location const &loc)
{
    if (this != &loc)
    {
        _path = loc._path;
        _root = loc._root;
        _autoindex = loc._autoindex;
        _index = loc._index;
        _alias = loc._alias;
        _allow_methods = loc._allow_methods;
        _return = loc._return;
        _cgi_ext = loc._cgi_ext;
        _cgi_path = loc._cgi_path;
        _client_max_body_size = loc._client_max_body_size;
    }
    return (*this);
}

Location::~Location()
{
    this->_index.clear();
    this->_allow_methods.clear();
    return ;
}

void    Location::initAllowMethods(bool input)
{
    _allow_methods["GET"] = input;
    _allow_methods["POST"] = input;
    _allow_methods["PUT"] = input;
    _allow_methods["DELETE"] = input;
    _allow_methods["HEAD"] = input;
    _allow_methods["CONNECT"] = input;
    _allow_methods["OPTIONS"] = input;
    _allow_methods["TRACE"] = input;
}

void    Location::parseLocation(std::string locationConfig)
{
    int                         index = 0;
    bool                        inBracket = false;
    std::string                 word;
    std::string                 currentParameter;
    std::vector<std::string>    storeValue;

    while (index < locationConfig.length())
    {
        if (ft_is_white_space(locationConfig[index]))
        {
            index++;
            continue;
        }

        word = findNextWord(locationConfig.substr(index, locationConfig.length()));
        std::cout << "currentParameter " << currentParameter << " word = " << word << " index: " << index << std::endl;
        if (currentParameter == "")
        {
            if (word == "location")
            {
                if (inBracket)
                {
                    std::string errorMessage = "Invalid syntax found location inside location" ;
                    storeValue.clear();
                    throw(errorMessage);
                }
                currentParameter = word;
                index += word.length();
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
                currentParameter = word;
                index += word.length();
            }
        } else {
            if (word == "{") { 
                if (currentParameter == "location") {
                    if (!setLocationParameter(currentParameter, storeValue))
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
                    inBracket = true;
                    storeValue.clear();
                    currentParameter = "";
                } else {
                    std::string errorMessage = "Invalid syntax: Found \'{\' in wrong place after ", currentParameter ;
                    throw(errorMessage);
                }
                    index += word.length();
            } else if (word == ";") {
                if (!setLocationParameter(currentParameter, storeValue))
                {
                    std::string errorMessage = "Cannot set value " + word;
                    std::vector<std::string>::iterator    it;

                    for (it = storeValue.begin(); it != storeValue.end(); it++)
                    {
                        errorMessage = errorMessage + " " + *it;
                    }

                    errorMessage = errorMessage + " in Parameter : " + currentParameter;
                    throw(errorMessage);
                }
                storeValue.clear();
                currentParameter = "";
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

bool Location::setLocationParameter(std::string param, std::vector<std::string> value)
{
    if (param == "location")
    {
        if (value.size() != 1)
        {
            return false;
        }

        this->_path = value[0];
        
    } else if (param == "alias") {
        if (value.size() != 1)
        {
            return false;
        }
        this->_alias = value[0];

    } else if (param == "allow_methods") {

        this->initAllowMethods(false);

        std::vector<std::string>::iterator itMethods;
        for (itMethods = value.begin(); itMethods != value.end(); itMethods++)
        {
            if (this->_allow_methods.find(*itMethods) == this->_allow_methods.end())
            {
                return false;
            } 
            this->_allow_methods[*itMethods] = true;
        }

    } else if (param == "return") {
        if (value.size() != 1)
        {
            return false;
        }
        this->_return = value[0];

    } else if (param == "cgi_ext") {
        if (value.size() < 1)
        {
            return false;
        }

        this->_cgi_ext = value;

    } else if (param == "cgi_path") {
        if (value.size() < 1)
        {
            return false;
        }

        this->_cgi_path = value;
        
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
            std::cout << "nani" << std::endl;
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

void Location::printLocationInfo(){
    std::cout << "===== Location " << this->_path << " =====" << std::endl;
    std::cout << "path: " << this->_path << std::endl;
    std::cout << "root: " << this->_root << std::endl;
    std::cout << "autoindex: " << this->_autoindex << std::endl;

    std::cout << "index: " << std::endl;
    std::vector<std::string>::iterator itIndex;
    for (itIndex = this->_index.begin(); itIndex < this->_index.end(); itIndex++){
        std::cout << "- " << *itIndex << std::endl;
    }

    std::cout << "allow_methods: " << std::endl;
    std::map<std::string, bool>::iterator itAllowMethods;
    for (itAllowMethods = this->_allow_methods.begin(); itAllowMethods != this->_allow_methods.end(); itAllowMethods++){
        if (itAllowMethods->second)
        {
            std::cout << "- " << itAllowMethods->first << std::endl;
        }
    }

    std::cout << "return: " << this->_return << std::endl;
    std::cout << "cgi_ext: " << std::endl;
    std::vector<std::string>::iterator itCgiExt;
    for (itCgiExt = this->_cgi_ext.begin(); itCgiExt < this->_cgi_ext.end(); itCgiExt++){
        std::cout << "- " << *itCgiExt << std::endl;
    }

    std::cout << "cgi_path: "  << std::endl;
    std::vector<std::string>::iterator itCgiPath;
    for (itCgiPath = this->_cgi_path.begin(); itCgiPath < this->_cgi_path.end(); itCgiPath++){
        std::cout << "- " << *itCgiPath << std::endl;
    }

    std::cout << "client_max_body_size: " << this->_client_max_body_size << std::endl;
}