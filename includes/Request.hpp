#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "utils.hpp"
# include "ErrorCode.hpp"
# include <utility>
# include <string>
# include <map>
# include <iostream>
# include <stdlib.h>
# define MAX_URI_LENGTH 4096

/*
    start-line
    *( header-field CRLF )
    CRLF
    [ message-body ]
*/

enum HttpMethods
{
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    NONE
};

class Request {
    public:
        Request();
        Request(Request const &req);
        ~Request();

        Request &operator=(Request const &req);

        bool                                isCompleted();
        bool                                parseRequest(std::string req);
        
        const std::string&                  getMethod() const;
        const std::string&                  getPath() const;
        const std::string&                  getQuery() const;
        const std::string&                  getVersion() const;
        int                                 getError();
        const std::map<std::string, std::string>&  getHeader() const;
        const std::string&                  getBody() const;

        void                                setMethod(std::string method);
        void                                setPath(std::string path);
        void                                setVersion(std::string version);
        void                                setError(int error);
        void                                setMaxBodySize(size_t max);
        void                                setBody(const std::string& newbody);

        void                                printRequest();
        void                                clear();
        bool                                keepAlive();

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
        size_t                              _max_body_size;

        bool                                handleChunked(std::string body);
};

#endif