#include "Response.hpp"

Response::Response()
{
    this->_response_content = "";
    this->_status = "";
    this->_error = 0;
    this->_body = "";
    this->_target_file = "";
}

Response::~Response()
{

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
        this->_header = response._header;
        this->_body = response._body;
        this->_target_file = response._target_file;
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
        this->_header = response._header;
        this->_body = response._body;
        this->_target_file = response._target_file;
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
    return ;
}

void        Response::buildResponse()
{
    if (this->_error)
    {
        buildErrorBody();
    }

    buildFirstLine();
    buildHeaders();
    buildBody();
}

void        Response::buildErrorBody()
{
    std::map<int, std::string> error_page = _server.getErrorPage();

    _target_file = _server.getRoot();
}

std::string Response::getResponse()
{
    return (this->_response_content);   
}

void        Response::buildFirstLine()
{
    _response_content = "HTTP/1.1 " + ft_to_string(_error) + " ";

    switch (_error)
    {
        case 100:
            _response_content.append("Continue"); break;
        case 101:
            _response_content.append("Switching Protocols"); break;
        case 200:
            _response_content.append("OK"); break;
        case 201:
            _response_content.append("Created"); break;
        case 202:
            _response_content.append("Accepted"); break;
        case 203:
            _response_content.append("Non-Authoritative Information"); break;
        case 204:
            _response_content.append("No Content"); break;
        case 205:
            _response_content.append("Reset Content"); break;
        case 206:
            _response_content.append("Partial Content"); break;
        case 300:
            _response_content.append("Multiple Choices"); break;
        case 301:
            _response_content.append("Moved Permanently"); break;
        case 302:
            _response_content.append("Found"); break;
        case 303:
            _response_content.append("See Other"); break;
        case 304:
            _response_content.append("Not Modified"); break;
        case 305:
            _response_content.append("Use Proxy"); break;
        case 307:
            _response_content.append("Temporary Redirect"); break;
        case 400:
            _response_content.append("Bad Request"); break;
        case 401:
            _response_content.append("Unauthorized"); break;
        case 402:
            _response_content.append("Payment Required"); break;
        case 403:
            _response_content.append("Forbidden"); break;
        case 404:
            _response_content.append("Not Found"); break;
        case 405:
            _response_content.append("Method Not Allowed"); break;
        case 406:
            _response_content.append("Not Acceptable"); break;
        case 407:
            _response_content.append("Proxy Authentication Required"); break;
        case 408:
            _response_content.append("Request Timeout"); break;
        case 409:
            _response_content.append("Conflict"); break;
        case 410:
            _response_content.append("Gone"); break;
        case 411:
            _response_content.append("Length Required"); break;
        case 412:
            _response_content.append("Precondition Failed"); break;
        case 413:
            _response_content.append("Payload Too Large"); break;
        case 414:
            _response_content.append("URI Too Long"); break;
        case 415:
            _response_content.append("Unsupported Media Type"); break;
        case 416:
            _response_content.append("Range Not Satisfiable"); break;
        case 417:
            _response_content.append("Expectation Failed"); break;
        case 426:
            _response_content.append("Upgrade Required"); break;
        case 500:
            _response_content.append("Internal Server Error"); break;
        case 501:
            _response_content.append("Not Implemented"); break;
        case 502:
            _response_content.append("Bad Gateway"); break;
        case 503:
            _response_content.append("Service Unavailable"); break;
        case 504:
            _response_content.append("Gateway Timeout"); break;
        case 505:
            _response_content.append("HTTP Version Not Supported"); break;
        default:
            _response_content.append("Default");
    }

    _response_content.append("\r\n");
}

void        Response::createHeaders()
{
    _header["Content-Type"] = "text/html"; // TODO Edit this
    _header["Content-Length"] = ft_to_string(_body.length());
}

void        Response::buildHeaders()
{
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

void        Response::buildBody()
{
    _response_content.append(_body);
}