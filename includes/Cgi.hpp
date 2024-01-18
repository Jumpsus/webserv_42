#ifndef CGI_HPP
# define CGI_HPP

# include <map>
# include <unistd.h>
# include "Request.hpp"
# include "config/Config.hpp"
# include "Webserv.hpp"
# define env_iter std::map<std::string, std::string>::const_iterator

/* refer to rfc3875 */
class CgiHandler {
    public:
        CgiHandler(Request& request, Location& loc_ptr);
        CgiHandler(CgiHandler const &cgi);
        ~CgiHandler();

        CgiHandler &operator=(CgiHandler const &cgi);

        std::string                         execCgi(const std::string& script); //execute script in cgi-bin and return body
        const std::string&                  getBody();
        void                                setBody(const std::string& ebody);
        void                                printEnv();

    private:
        std::map<std::string, std::string>  _env;
        std::string                         _body;

        void                                _setEnv(Request& request, Location& loc_ptr);
        char**                              _envToCstrArr() const;
};


#endif