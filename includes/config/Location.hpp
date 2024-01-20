#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <vector>
# include "config/Server.hpp"

class Server;

/* syntax: https://nginx.org/en/docs/http/ngx_http_core_module.html#location */
class Location {
    public:
        Location();
        Location(std::string const locationConfig ,Server const &serv);
        Location(Location const &loc);
        Location &operator=(Location const &loc);
        ~Location();

        void printLocationInfo();

        /*getters*/
        const std::string&                  getPath() const;
        const std::string&                  getRoot() const;
        const std::vector<std::string>&     getCgiPath() const;
        const std::vector<std::string>&     getCgiExt() const;
        const size_t&                       getClientMaxBodySize() const;
        const std::map<std::string, bool>&  getAllowMethod() const;
        const std::string&                  getReturn() const;
        const std::string&                  getAlias() const;
    private:
        std::string                     _path;
        std::string                     _root;
        bool                            _autoindex;
        std::vector<std::string>        _index;
        std::string                     _alias;
        std::map<std::string, bool>     _allow_methods;
        std::string                     _return;
        std::vector<std::string>        _cgi_ext;
        std::vector<std::string>        _cgi_path;
        size_t                          _client_max_body_size;

        void                            initAllowMethods(bool input);
        void                            parseLocation(std::string locationConfig);
        bool                            setLocationParameter(std::string param, std::vector<std::string> value);
};

#endif