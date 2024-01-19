#include "Response.hpp"

Response::Response()
{
    this->_response_content = "";
    this->_status = 0;
    this->_error = 0;
    this->_auto_index = false;
    this->_body = "";
    this->_target_file = "";
}

Response::~Response()
{
    _header.clear();
    _error_map.clear();
}

Response::Response(Response const &response)
{
    if (this != &response)
    {
        this->_request = response._request;
        this->_server = response._server;
        this->_response_content = response._response_content;
        this->_status = response._status;
        this->_error = response._error;
        this->_auto_index = response._auto_index;
        this->_header = response._header;
        this->_body = response._body;
        this->_target_file = response._target_file;
        this->_error_map = response._error_map;
    }

    return ;
}

Response &Response::operator=(Response const &response)
{
    if (this != &response)
    {
        this->_request = response._request;
        this->_server = response._server;
        this->_response_content = response._response_content;
        this->_status = response._status;
        this->_error = response._error;
        this->_auto_index = response._auto_index;
        this->_header = response._header;
        this->_body = response._body;
        this->_target_file = response._target_file;
        this->_error_map = response._error_map;
    }

    return (*this);
}

void        Response::setRequest(Request req)
{
    this->_request = req;
    this->_error = req.getError();
    return ;
}

void        Response::setServer(Server serv)
{
    this->_server = serv;
    this->_error_map = serv.getErrorPage();
    return ;
}

void        Response::buildResponse()
{
    if (this->_error || buildBody())
    {
        _status = _error;
        buildErrorBody();
    }

    appendFirstLine();
    appendHeaders();
    appendBody();
}

int         Response::buildBody()
{
    if (findMatchLocation())
    {
        // Case use parameter from location in server;
    } else {
        // Case use parameter from server;
        _target_file = _server.getRoot() + _request.getPath();

        if (!readFile(_target_file, _body))
        {
            _error = 404;
            return (1);
        }

        _status = 200;
    }

    return (0);
}

void        Response::buildErrorBody()
{
    _body = "";
    // if (_error_page.count(_error) > 0)
    // {
    //     return ;
    // }

    setDefaultErrorFile(_error);
    readFile(_target_file, _body);
}

void        Response::setDefaultErrorFile(int error)
{
    std::string file_name = "docs/default-error/";
    file_name.append(ft_to_string(error));
    file_name.append(".html");

    _target_file = file_name;
}

std::string Response::getResponse()
{
    return (this->_response_content);   
}

void        Response::appendFirstLine()
{
    _response_content = "HTTP/1.1 " + ft_to_string(_status) + " ";
    _response_content.append(mapStatusCode(_status));
    _response_content.append("\r\n");
}

void        Response::createHeaders()
{
    _header["Content-Type"] = "text/html"; // TODO Edit this
    _header["Content-Length"] = ft_to_string(_body.length());
}

void        Response::appendHeaders()
{
    createHeaders();
    
    std::map<std::string, std::string>::iterator it;

    for (it = _header.begin(); it != _header.end(); it ++)
    {
        _response_content.append(it->first);
        _response_content.append(": ");
        _response_content.append(it->second);
        _response_content.append("\r\n");
    }
    _response_content.append("\r\n");
}

void        Response::appendBody()
{
    _response_content.append(_body);
    std::cout << "body = " << _body << std::endl;
}

bool        Response::findMatchLocation()
{
    std::vector<Location>           locs = _server.getLocations();
    
    for (std::vector<Location>::iterator it = locs.begin(); it != locs.end() ;it++)
    {
        if (_request.getPath().find(it->getPath()) == 0)
        {
            if (it->getPath().length() > _location.length())
            {
                _location = it->getPath();
            }
        }
    }

    return (_location.length() > 0);
}

void        Response::printResponse()
{
    std::cout << _response_content << std::endl;
}

void        Response::clear()
{
    this->_response_content = "";
    this->_status = 0;
    this->_error = 0;
    this->_body = "";
    this->_target_file = "";
    _request.clear();
}