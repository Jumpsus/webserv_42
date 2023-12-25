#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map> 
/* syntax: https://nginx.org/en/docs/http/ngx_http_core_module.html#location */
class Location {
    public:
        Location();
        Location(Location const &loc);
        Location &operator=(Location const &loc);
        ~Location();

        void printLocationInfo();
    private:
        std::string                     _path;
        std::string                     _root;
        bool                            _autoindex;
        std::string                     _index;
        std::string                     _alias;
        std::map<std::string, bool>     _allowed_methods;
        std::string                     _return;
        std::string                     _cgi_ext;
        std::string                     _cgi_path;
        size_t                          _client_max_body_size;
};

#endif