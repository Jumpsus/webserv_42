#include "Request.hpp"

Request::Request()
{
    _body = "";
    _body_type = "";
    _body_length = 0;
    _method = "";
    _path = "";
    _query = "";
    _fragment = "";
    _version = "";
    _protocol = "";
    _protocol_version = "";
    _connection = "";
    _error = 0;
    _multipart = false;
    _boundary = "";
    _completed = false;
}

Request::Request(Request const &req)
{
    if (this != &req)
    {
        this->_body = req._body;
        this->_body_type = req._body_type;
        this->_body_length = req._body_length;
        this->_method = req._method;
        this->_path = req._path;
        this->_query = req._query;
        this->_fragment = req._fragment;
        this->_version = req._version;
        this->_protocol = req._protocol;
        this->_protocol_version = req._protocol_version;
        this->_connection = req._connection;
        this->_max_body_size = req._max_body_size;
        this->_multipart = req._multipart;
        this->_boundary = req._boundary;
        this->_error = req._error;
        this->_header = req._header;
        this->_completed = req._completed;
    }
}

Request::~Request()
{
    this->_header.clear();
}

Request& Request::operator=(Request const &req)
{
    if (this != &req)
    {
        this->_body = req._body;
        this->_body_type = req._body_type;
        this->_body_length = req._body_length;
        this->_method = req._method;
        this->_path = req._path;
        this->_query = req._query;
        this->_fragment = req._fragment;
        this->_version = req._version;
        this->_protocol = req._protocol;
        this->_protocol_version = req._protocol_version;
        this->_connection = req._connection;
        this->_max_body_size = req._max_body_size;
        this->_multipart = req._multipart;
        this->_boundary = req._boundary;
        this->_error = req._error;
        this->_header = req._header;
        this->_completed = req._completed;
    }

    return (*this);
}

bool    Request::isCompleted()
{
    return (this->_completed);
}

bool    Request::isMultipart()
{
    return (this->_multipart);
}

/* https://www.rfc-editor.org/rfc/rfc7230.html#section-3 */
/* return true case parse completed , false case parse incomplete */
bool    Request::parseRequest(std::string req)
{
    std::string                         buffer;
    std::string                         line;
    std::size_t                         found;
    std::pair<std::string, std::string> tempHeader;
    bool                                completed_header = false;

    /* handle request-line (3.1.1) 
       method SP request-target SP HTTP-version CRLF */
    found = req.find("\r\n");
    if (found == std::string::npos)
    {
        this->_completed = false;
        return false;
    }
    line = req.substr(0, found);

    this->setMethod(splitString(line, " "));
    this->setPath(splitString(line, " "));
    this->setVersion(line);

    if (this->_error != 0)
    {
        this->_completed = true;
        return true;
    }

    buffer = req.substr(found + 2, req.length());

    /* handle header-field (3.2) */

    do {
        found = buffer.find("\r\n");
        if (found == std::string::npos)
        {
            this->_completed = false;
            return false;
        }
        if (found == 0)
        {
            buffer = buffer.substr(found + 2, buffer.length());
            completed_header = true;
            break;
        }
        line = buffer.substr(0, found);
        tempHeader = parseHeader(line);
        
        /* case invalid header key */
        if (tempHeader.first == "")
        {
            this->setError(400);
            this->_completed = true;
            return true;
        }
        this->_header.insert(tempHeader);
        buffer = buffer.substr(found + 2, buffer.length());
    } while (buffer.length() > 0);

    if (!completed_header)
    {
        this->_completed = false;
        return false;
    }

    /* handle message-body (3.3) */
    if (_header.count("content-length") > 0)
    {
        if (!ft_isdigit(_header["content-length"]))
        {
            std::cout << "content-length is not digit " <<  _header["content-length"] <<std::endl;
            for (size_t i = 0; i < _header["content-length"].length(); i++)
            {
                int x = _header["content-length"][i];
                std::cout << x << " " << std::endl;
            }
            this->setError(400);
            this->_completed = true;
            return true;
        }
        _body_length = ft_stoi(_header["content-length"]);
        _body_type = "body";
    }

    if (_header.count("transfer-encoding") > 0)
    {
        _body_type = "body";
        if (_header["transfer-encoding"].find("chuncked") != std::string::npos)
        {
            /* in chucked body length is detemine dynamically */
             _body_type = "chuncked";
             _body_length = 0;
        }
    }

    if (_header.count("content-type") > 0)
    {
        if (_header["content-type"].find("multipart/form-data") != std::string::npos)
        {
            size_t found = _header["content-type"].find("boundary=");
            if (found != std::string::npos)
            {
                _boundary = _header["content-type"].substr(found + 9, _header["content-type"].length());
            }
            _multipart = true;
        }
    }

    if (_header.count("connection") > 0)
    {
        _connection = _connection.append(_header["connection"]);
    }

    if (_body_type == "body" && buffer.length() < _body_length)
    {
        this->_completed = false;
        return false;
    } else {
        this->_body = buffer;
        this->_completed = true;
        return true;
    }

    if (_body_type == "chuncked")
    {
        if (!handleChunked(buffer))
        {
            this->_completed = false;
            return false;
        }
        this->_completed = true;
        return true;
    }

    this->_completed = true;
    return true;
}

/*
Chunck handling
    length := 0
     read chunk-size, chunk-ext (if any), and CRLF
     while (chunk-size > 0) {
        read chunk-data and CRLF
        append chunk-data to decoded-body
        length := length + chunk-size
        read chunk-size, chunk-ext (if any), and CRLF
     }
     read trailer field
     while (trailer field is not empty) {
        if (trailer field is allowed to be sent in a trailer) {
            append trailer field to existing header fields
        }
        read trailer-field
     }
     Content-Length := length
*/

bool        Request::handleChunked(std::string body)
{
    size_t          len = 0;
    size_t          found = 0;
    std::string     buffer = body;
    std::string     temp;
    bool            is_chuck_len = true;

    do {
        /* find length */
        found = buffer.find("\r\n");
        if (found == std::string::npos)
        {
            return false;
        }

        if (is_chuck_len)
        {
            std::string temp = ft_tolower(buffer.substr(0, found));

            if (!ft_ishex(temp))
            {
                setError(400);
                return true;
            }

            len = ft_htoi(temp);
            _body_length += len;
            is_chuck_len = true;
        } else {
            std::string temp = buffer.substr(0, found);

            if (temp.length() != len)
            {
                setError(400);
                return true;
            }

            _body = _body + temp;
            is_chuck_len = true;
        }
        
        buffer = buffer.substr(found + 2, buffer.length());

    } while (len > 0);

    found = buffer.find("\r\n");
    if (found != 0)
    {
        setError(400);
        return true;
    }

    return true;
}

const std::string& Request::getMethod() const
{
    return (this->_method);
}

const std::string& Request::getPath() const
{
    return (this->_path);
}

const std::string& Request::getQuery() const
{
    return (this->_query);
}

const std::map<std::string, std::string>& Request::getHeader() const
{
    return (this->_header);
}

int Request::getError()
{
    return (this->_error);
}

const std::string& Request::getVersion() const
{
    return (this->_version);
}

const std::string& Request::getBody() const
{
    return (this->_body);
}

const std::string& Request::getBoundary() const
{
    return (this->_boundary);
}

void    Request::setBody(const std::string& newbody)
{
    this->_body = newbody;
}

void    Request::setPath(std::string uri)
{
    std::size_t     found;
    std::string     temp;
    
    if (uri == "")
    {
        this->setError(400);
        return ;
    } else if (uri[0] != '/') {
        this->setError(400);
        return ;
    }
    else if (ft_isURI(uri) == false) {
        this->setError(400);
        return ;
    }
    else if (uri.length() > MAX_URI_LENGTH) {
        this->setError(414);
        return ;
    }
    found = uri.find('?');
    if (found == std::string::npos)
    {
        found = uri.find('#');
        if (found == std::string::npos) {
            this->_path = uri;
        } else {
            this->_path = uri.substr(0, found);
            this->_fragment = uri.substr(found + 1, uri.length());
        }
    } else {
        this->_path = uri.substr(0, found);
        temp = uri.substr(found + 1, uri.length());
        found = temp.find('#');
        if (found == std::string::npos) {
            this->_query = temp;
        } else {
            this->_query = temp.substr(0, found);
            this->_fragment = temp.substr(found + 1, uri.length());
        }
    }
}

void Request::setMethod(std::string method)
{
    method = ft_toupper(method);

    if (method == "GET")
    {
        this->_method = "GET";
        return;
    } else if (method == "HEAD") {
        this->_method = "HEAD";
        return;
    } else if (method == "POST") {
        this->_method = "POST";
        return;
    } else if (method == "PUT") {
        this->_method = "PUT";
        return;
    } else if (method == "DELETE") {
        this->_method = "DELETE";
        return;
    }
    this->setError(501); //its not 400
    return ;
}

void    Request::setVersion(std::string version)
{
    std::size_t     found;
    std::string     protocol;
    std::string     protocol_version;
    int             major;
    int             minor;

    found = version.find('/');

    if (found == std::string::npos)
    {
        this->setError(400);
        return ;
    }
    protocol = version.substr(0, found);
    protocol_version = version.substr(found + 1, protocol.length());

    if (protocol != "HTTP")
    {
        this->setError(400);
        return ;
    }

    found = protocol_version.find('.');
    if (found == std::string::npos)
    {
        this->setError(400);
        return ;
    }

    if (!ft_isdigit(protocol_version.substr(0, found)) || !ft_isdigit(protocol_version.substr(found + 1, protocol_version.length())) ||
        protocol_version.substr(0, found).length() == 0 || protocol_version.substr(found + 1, protocol_version.length()).length() == 0)
    {
        this->setError(400);
        return ;
    }

    major = ft_stoi(protocol_version.substr(0, found));
    minor = ft_stoi(protocol_version.substr(found + 1, protocol_version.length()));

    if (major > 1 || minor > 1)
    {
        this->setError(505);
        return ;
    }

    this->_version = version;
    this->_protocol = protocol;
    this->_protocol_version = protocol_version;
}

void Request::setError(int error)
{
    this->_error = error;
    return ;
}

void Request::setMaxBodySize(size_t max)
{
    this->_max_body_size = max;
    return ;
}

void Request::printRequest()
{
    std::map<std::string, std::string>::iterator it;

    std::cout << "--- header ---" << std::endl;
    for (it = this->_header.begin(); it != this->_header.end(); it++)
    {
        std::cout << it->first << " = " << it->second << std::endl;
    }

    std::cout << "--- body ---" << std::endl;
    std::cout << this->_body << std::endl;
    std::cout << "--- query ---" << std::endl;
    std::cout << this->_query << std::endl;
}

void Request::clear()
{
    _header.clear();
    _body = "";
    _body_type = "";
    _body_length = 0;
    _method = "";
    _path = "";
    _query = "";
    _fragment = "";
    _version = "";
    _protocol = "";
    _protocol_version = "";
    _connection = "";
    _error = 0;
    _max_body_size = 0;
    _multipart = false;
    _boundary = "";
    _completed = false;
}

/*
    keep-alive is default in HTTP/1.1
    https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Connection 
*/
bool Request::keepAlive()
{
    if (_connection.find("close") != std::string::npos)
        return false;
    return true;
}