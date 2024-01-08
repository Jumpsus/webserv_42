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

}

Response &Response::operator=(Response const &response)
{
    return (*this);
}

// std::string Response::getResponse()
// {
//     return "v";   
// }