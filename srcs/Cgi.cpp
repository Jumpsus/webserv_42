#include "Cgi.hpp"

CgiHandler::CgiHandler()
{

}

CgiHandler::CgiHandler(CgiHandler const &cgi)
{
    if (this != &cgi)
    {
        this->_env = cgi._env;
        this->_body = cgi._body;
    }
}

CgiHandler &CgiHandler::operator=(CgiHandler const &cgi)
{
    if (this != &cgi)
    {
        this->_env = cgi._env;
        this->_body = cgi._body;
    }

    return (*this);
}

CgiHandler::~CgiHandler()
{

}

void CgiHandler::setCgiHandler(Request req, Config conf)
{
    std::map<std::string, std::string> headers = req.getHeader();

    /* The following environment variables are not request-specific 
        and are set for all requests */
    this->_env["SERVER_SOFTWARE"] = "WebServ/1.0";
    this->_env["SERVER_NAME"] = "";
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";

    /* The following environment variables are specific to 
       the request being fulfilled by the gateway program */
    this->_env["SERVER_PROTOCOL"] = req.getVersion();
    this->_env["SERVER_PORT"] = "";
    this->_env["REQUEST_METHOD"] = req.getMethod();
    this->_env["PATH_INFO"] = req.getPath();
    this->_env["PATH_TRANSLATED"] = req.getPath();
    this->_env["SCRIPT_NAME"] = "";
    this->_env["QUERY_STRING"] = req.getQuery();
    this->_env["REMOTE_HOST"] = "";
    this->_env["REMOTE_ADDR"] = "";
    this->_env["AUTH_TYPE"] = "";
    this->_env["REMOTE_USER"] = "";
    this->_env["REMOTE_IDENT"] = "";
    this->_env["CONTENT_TYPE"] = headers["Content-Type"];
    this->_env["CONTENT_LENGTH"] = ft_to_string(req.getBody().length());
    
    this->_env["HTTP_ACCEPT"] = "";
    this->_env["HTTP_USER_AGENT"] = "";

    this->_body = req.getBody();
}