#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <map>

class Response {
    public:
        Response();
        Response(Response const &res);
        ~Response();

        Response &operator=(Response const &res);
        std::string     getResponse();
    private:
        std::string                         _response;
        std::string                         _status;
        int                                 _error;
        std::string                         _version;
        std::string                         _path;
        std::map<std::string, std::string>  _header;
        std::string                         _body;
};

#endif