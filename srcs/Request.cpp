#include "Request.hpp"

Request::Request()
{
    _body = "";
    _method = "";
    _path = "";
    _query = "";
    _fragment = "";
    _version = "";
    _protocol = "";
    _protocol_version = "";
    _connection = "";
    _error = 0;
}

Request::Request(Request const &req)
{
    if (this != &req)
    {
        this->_body = req._body;
        this->_method = req._method;
        this->_path = req._path;
        this->_query = req._query;
        this->_fragment = req._fragment;
        this->_version = req._version;
        this->_protocol = req._protocol;
        this->_protocol_version = req._protocol_version;
        this->_connection = req._connection;
        this->_error = req._error;
        this->_header = req._header;
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
        this->_method = req._method;
        this->_path = req._path;
        this->_query = req._query;
        this->_fragment = req._fragment;
        this->_version = req._version;
        this->_protocol = req._protocol;
        this->_protocol_version = req._protocol_version;
        this->_connection = req._connection;
        this->_error = req._error;
        this->_header = req._header;
    }

    return (*this);
}

/* https://www.rfc-editor.org/rfc/rfc7230.html#section-3 */
bool    Request::parseRequest(std::string req)
{
    std::string                         buffer;
    std::string                         line;
    std::size_t                         found;
    std::pair<std::string, std::string> tempHeader;

    /* handle request-line (3.1.1) 
       method SP request-target SP HTTP-version CRLF */
    found = req.find("\r\n");
    if (found == std::string::npos)
    {
        this->setError(400);
        return false;
    }
    line = req.substr(0, found);

    this->setMethod(splitString(line, " "));
    this->setPath(splitString(line, " "));
    this->setVersion(line);

    if (this->_error != 0)
    {
        return false;
    }

    buffer = req.substr(found + 2, req.length());

    /* handle header-field (3.2) */
    while (buffer.length() > 0)
    {
        found = buffer.find("\r\n");
        if (found == std::string::npos)
        {
            this->setError(400);
            return false;
        }
        if (found == 0)
        {
            buffer = buffer.substr(found + 2, buffer.length());
            break;
        }
        line = buffer.substr(0, found);
        tempHeader = parseHeader(line);
        // std::cout << "line= " << line << ", first= " << tempHeader.first << ", second= " << tempHeader.second << std::endl;
        this->_header.insert(tempHeader);
        buffer = buffer.substr(found + 2, buffer.length());
    }

    /* handle message-body (3.3) */
    this->_body = buffer;
    return true;
}

std::string Request::getMethod()
{
    return (this->_method);
}

std::string Request::getPath()
{
    return (this->_path);
}

std::string Request::getQuery()
{
    return (this->_query);
}

std::map<std::string, std::string> Request::getHeader()
{
    return (this->_header);
}

int Request::getError()
{
    return (this->_error);
}

std::string Request::getVersion()
{
    return (this->_version);
}

std::string Request::getBody()
{
    return (this->_body);
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
    this->setError(400);
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
}
