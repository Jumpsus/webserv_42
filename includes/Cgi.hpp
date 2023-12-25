#ifndef CGI_HPP
# define CGI_HPP

# include <map>
# include <unistd.h>
# include "Request.hpp"
# include "config/Config.hpp"

/* refer to rfc3875 */
class CgiHandler {
    public:
        CgiHandler();
        CgiHandler(CgiHandler const &cgi);
        ~CgiHandler();

        CgiHandler &operator=(CgiHandler const &cgi);

       void setCgiHandler(Request req, Config conf); // TODO

    private:
        std::map<std::string, std::string>  _env;
        std::string                         _body;

};


#endif