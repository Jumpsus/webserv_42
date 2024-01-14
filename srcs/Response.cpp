#include "Response.hpp"

Response::Response()
{
    _response = "";
}

Response::~Response()
{

}

Response::Response(Response const &response)
{
    _response;
    _status;
    _error;
    _version;
    _path;
    _header;
    _body;
}

Response &Response::operator=(Response const &response)
{
    return (*this);
}

// std::string Response::getResponse()
// {
//     return "v";   
// }