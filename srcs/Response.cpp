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
    Location    loc;

    if (findMatchLocation(loc))
    {
        // Case use parameter from location in server;

        // check max body size 
        if (loc.getClientMaxBodySize() < _request.getBody().length())
        {
            _error = 413;
            return (1);
        }

        // check allow method
        if (!loc.getAllowMethod().at(_request.getMethod()))
        {
            _error = 405;
            return (1);
        }

        // check return
        if (!loc.getReturn().empty())
        {
            _location = loc.getReturn();
            if (_location[0] != '/')
            {
                _location = "/" + _location;
            }
            _error = 301;
            return (1);
        }

        //handle alias
        if (!loc.getAlias().empty())
        {
            _target_file = ft_join(loc.getAlias(), _request.getPath().substr(0, loc.getPath().length()));
        } else {
            _target_file = ft_join(loc.getRoot(), _request.getPath());
        }

        // auto index 
        /*
            https://www.oreilly.com/library/view/nginx-http-server/
            9781788623551/052b1381-d911-46df-8bd2-1bebf70f44b8.xhtml
            #:~:text=Description,Syntax%3A%20on%20or%20off
        */

        if (isDirectory(_target_file))
        {
            if (_target_file[_target_file.length() - 1] != '/')
            {
                _status = 301;
                _location = _request.getPath() + "/";
                return (1);
            }

            if (!isFileExists(_target_file + loc.getIndex()))
            {
                if (loc.getAutoIndex())
                {
                    // TODO: handle auto index
                    // _body = "";
                    return (0);
                } else {
                    _error = 403;
                    return (1);
                }
            }

            _target_file = _target_file + loc.getIndex();
        }

        if (loc.getCgiExt().size() != 0)
        {
            std::string file_extension = "." + getExtension(_request.getPath());
            std::vector<std::string> cgi_ext = loc.getCgiExt();

            for (std::vector<std::string>::iterator it = cgi_ext.begin(); it != cgi_ext.end(); it++)
            {
                if (file_extension == (*it))
                {
                    // TODO: handle cgi
                }
            }
        }

    } else {
        // Case use parameter from server;
        _target_file = ft_join(_server.getRoot(), _request.getPath());

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
    if (_error_map.count(_error) > 0 && (_error >= 400 && _error < 500))
    {
        _location =  _error_map[_error];
        if (_location[0] != '/')
        {
            _location = "/" + _location;
        }
        _status = 302;
        return ;
    } else {
        _body = defaultErrorPage(_error);
    }
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
    _header["Content-Type"] = mapContentType(_target_file);
    _header["Content-Length"] = ft_to_string(_body.length());
    _header["Location"] = _location;

    if (_request.getHeader().count("connection") != 0)
       _header["Connection"] = _request.getHeader().at("connection");
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

bool        Response::findMatchLocation(Location &loc)
{
    std::vector<Location>           locs = _server.getLocations();
    
    for (std::vector<Location>::iterator it = locs.begin(); it != locs.end() ;it++)
    {
        if (_request.getPath().find(it->getPath()) == 0)
        {
            if (it->getPath().length() > _location.length())
            {
                _location = it->getPath();
                loc = *it;
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
    this->_location = "";
    _request.clear();
}