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
        CgiHandler();
        CgiHandler(Request& request, Location& loc_ptr);
        CgiHandler(CgiHandler const &cgi);
        ~CgiHandler();

        CgiHandler &operator=(CgiHandler const &cgi);

        std::string                         execCgi(const std::string& script, int &error);
        const std::string&                  getBody();
        void                                setBody(const std::string& ebody);
        void                                printEnv();

    private:
        std::map<std::string, std::string>  _env;
        std::string                         _body;

        std::string                         _getPathInfo(const std::string& req_path, const std::vector<std::string>& exts);
        std::string                         _decodeQuery(std::string req_query);
        void                                _setEnv(Request& request, Location& loc_ptr);
        char**                              _envToCstrArr() const;
};


#endif