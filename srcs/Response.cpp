#include "Response.hpp"

Response::Response()
{
    this->_response_content = "";
    this->_status = 0;
    this->_error = 0;
    this->_auto_index = false;
    this->_body = "";
    this->_target_file = "";
    this->_cgi_status = false;
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
        this->_cgi_status = response._cgi_status;
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
        this->_cgi_status = response._cgi_status;
    }

    return (*this);
}

std::string Response::getResponse()
{
    return (this->_response_content);   
}

std::string Response::getBody()
{
    return this->_body;
}

void        Response::setBody(const std::string& new_body)
{
    this->_body = new_body;
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

void        Response::setError(int error_code)
{
    this->_error = error_code;
}

bool        Response::getCgiStatus()
{
    return this->_cgi_status;
}

void        Response::switchCgiOff()
{
    if (this->_cgi_status == true)
        this->_cgi_status = false;
}

void        Response::buildResponse()
{
    std::cout << "build response\n";
    if (this->_error || buildBody())
    {
        _status = _error;
        buildErrorBody();
    }
    appendFirstLine();
    appendHeaders();
    appendBody();
}

void        Response::editResponseToCgi()
{
    _response_content.clear();
    int         body_len = _body.length();
    std::string body_buff;
    std::stringstream   ssb(_body);
    int                 del_pos = 0;
    while (getline(ssb, body_buff,'\n')) {
        if (body_buff.find("Content-type:") != std::string::npos && body_buff.find("html") != std::string::npos)
            _target_file = ".html";
        if (body_buff.length() == 0)
            del_pos++;
        if (body_buff != "<!DOCTYPE html>" && body_buff != "<html>")
            del_pos += body_buff.length();
        else
            break ;
    }
    _body = _body.substr(del_pos, body_len - del_pos);
    appendFirstLine();
    appendHeaders();
    appendBody();
}

int         Response::buildBody()
{
    std::cout << "build body\n";
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
            _target_file = ft_join(loc.getAlias(), _request.getPath().substr(loc.getPath().length()));
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

            std::vector<std::string> indexs = loc.getIndex();
            bool                     file_exist = false;

            for (std::vector<std::string>::iterator it = indexs.begin(); it != indexs.end(); it++)
            {
                if (isFileExists(_target_file + *it))
                {
                    _target_file = _target_file + *it;
                    file_exist = true;
                    break;
                }
            }

            if (!file_exist)
            {
                if (loc.getAutoIndex())
                {
                    // TODO: handle auto index
                    _body = buildHtmlIndex(_target_file);
                    if (_body == "") {
                        _error = 500;
                        return (1);
                    }
                    return (0);
                } else {
                    _error = 403;
                    return (1);
                }
            }
        }

        if (loc.getCgiExt().size() != 0)
        {
            return (handleCgi(_target_file, loc));
        }

    } else {
        // Case use parameter from server;
        _target_file = ft_join(_server.getRoot(), _request.getPath());
        //std::cout << "_target_file = " << _target_file << std::endl;

        if (isDirectory(_target_file))
        {
            if (_target_file[_target_file.length() - 1] != '/')
            {
                _status = 301;
                _location = _request.getPath() + "/";
                return (1);
            }

            std::vector<std::string> indexs = _server.getIndex();
            bool                     file_exist = false;

            for (std::vector<std::string>::iterator it = indexs.begin(); it != indexs.end(); it++)
            {
                if (isFileExists(_target_file + *it))
                {
                    _target_file = _target_file + *it;
                    file_exist = true;
                    break;
                }
            }

            if (!file_exist)
            {
                if (_server.getAutoIndex())
                {
                    // TODO: handle auto index
                    _body = buildHtmlIndex(_target_file);
                    if (_body == "") {
                        _error = 500;
                        return (1);
                    }
                    return (0);
                } else {
                    _error = 403;
                    return (1);
                }
            }
        }
    }

    // handle request type

    std::string method = ft_toupper(_request.getMethod());

    if (method == "GET" || method == "HEAD")
    {
        if (!readFile(_target_file, _body))
        {
            std::cout << "target = " << _target_file << std::endl;
            _error = 404;
            return (1);
        }
    } else if (method == "POST" || method == "PUT") {
        /* https://datatracker.ietf.org/doc/html/rfc7231#section-4.3.3 */
        if (isFileExists(_target_file) && method == "POST")
        {
            _status = 204;
            return (0);
        }

        std::ofstream file(_target_file.c_str(), std::ios::binary);
        if (file.fail())
        {
            _error = 404;
            return (1);
        }

        file << _request.getBody();
        file.close();

    } else if (method == "DELETE") {
        // remove file 
        if (!isFileExists(_target_file))
        {
            _error = 404;
            return (1);
        }
        if (remove(_target_file.c_str()) != 0)
        {
            _error = 500;
            return (1);
        }
    }

    _status = 200;
    return (0);
}

int Response::handleCgi(const std::string& tg, Location& loc)
{
    std::cout << "handle cgi\n";
    if (tg.find("cgi-bin") == std::string::npos)
        return 1;
    std::string ext = tg.substr(tg.find("cgi-bin"));
    //std::cout << "ext = " << ext << std::endl;
    std::string file_extension = "." + getExtension(tg);
    std::vector<std::string> cgi_ext = loc.getCgiExt();
    //std::cout << "cgi body = " << this->_request.getBody() << std::endl;

    for (std::vector<std::string>::iterator it = cgi_ext.begin(); it != cgi_ext.end(); it++)
    {
        if (file_extension == (*it))
        {
            cgi.clear();
            cgi.setArgs0(*it, loc);
            cgi.setCgiPath(ext);
            if (cgi.initPipes(_error) == true)
                this->_cgi_status = true;
            cgi.setEnv(this->_request, loc);
            cgi.execCgi(_error, _status);
            if (_error != 0) {
                _status = _error;
                return (1);
            }
        }
    }
    _status = 200;
    return 0;
}

std::string Response::buildHtmlIndex(const std::string& tar_dir)
{
    std::cout << "build html index\n";
    std::string html;
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(tar_dir.c_str())) != NULL)
    {
        /*make css format header*/
        html.append("<!DOCTYPE html>\n<html>\n\t<head>\n");
        html.append("\t\t<title> index of " + tar_dir + "<title>\n");
        html.append("\t\t<style>\n\t\t\ttable, th, td{\n\t\t\t\tborder-collapse: collapse;\n\t\t\t}\n");
        html.append("\t\t\tth, td{\n\t\t\t\tpadding: 5px;\n\t\t\t}\n");
        html.append("\t\t\tth {\n\t\t\t\ttext-align: left;\n\t\t}\n");
        html.append("\t\t</style>\n\t</head>\n\n");

        /*make bold header and table index*/
        html.append("\t<body>\n\t<h1> Index of " + tar_dir + "</h1>\n");
        html.append("\t\t<table style=\"width:100%; font-size: 15px\">\n");
        html.append("\t\t\t<tr>\n\t\t\t\t<th style=\"width:60%\">File Name</th>\n");
        html.append("\t\t\t\t<th style=\"width:300\">Last Modification</th>\n");
        html.append("\t\t\t\t<th style=\"width:100\">File Size</th>\n");
        html.append("\t\t\t</tr>\n");

        /*get all file within the directory*/
        while ((ent = readdir(dir)) != NULL)
        {
            struct stat file_stat;
            std::string file_name = ent->d_name;
            stat(file_name.c_str(), &file_stat);

            //append file name
            html.append("\t\t\t<tr>\n\t\t\t\t<td>" + file_name);
            if (S_ISDIR(file_stat.st_mode))
                html.append("/");
            html.append("</td>\n");

            //append file last modified time
            html.append("\t\t\t\t<td>");
            html.append(ctime(&file_stat.st_mtime));
            html.append("</td>\n");

            //append file size
            html.append("\t\t\t\t<td>");
            if (S_ISDIR(file_stat.st_mode))
                html.append("-");
            else
                html.append(ft_to_string(file_stat.st_size));
            html.append("</td>\n\t\t\t</tr>");
        }
        closedir(dir);
        html.append("\t\t<table>\n\t</body>\n</html>\n");
        return html;
    }
    else
        return "";
}

void        Response::buildErrorBody()
{
    std::cout << "build error body\n";
    _body = "";
    _target_file = ".html";
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
    this->_cgi_status = false;
    _request.clear();
}