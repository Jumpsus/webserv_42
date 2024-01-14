#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "utils.hpp"
# include "ErrorCode.hpp"
# include <utility>
# include <string>
# include <map>
# include <iostream>
# include <stdlib.h>

/*
    start-line
    *( header-field CRLF )
    CRLF
    [ message-body ]
*/

class Request {
    public:
        Request();
        Request(Request const &req);
        ~Request();

        Request &operator=(Request const &req);

        bool                                isCompleted();
        bool                                parseRequest(std::string req);
        
        std::string                         getMethod();
        std::string                         getPath();
        std::string                         getQuery();
        std::string                         getVersion();
        int                                 getError();
        std::map<std::string, std::string>  getHeader();
        std::string                         getBody();
        
        void                                setMethod(std::string method);
        void                                setPath(std::string path);
        void                                setVersion(std::string version);
        void                                setError(int error);

        void                                printRequest();

    private:
        bool                                _completed;
        std::map<std::string, std::string>  _header;
        std::string                         _body;
        std::string                         _body_type;
        size_t                              _body_length;
        std::string                         _method;
        std::string                         _path;
        std::string                         _query;
        std::string                         _fragment;
        std::string                         _version;
        std::string                         _protocol;
        std::string                         _protocol_version;
        std::string                         _connection;
        int                                 _error;

        bool                                handleChunked(std::string body);
};

#endif