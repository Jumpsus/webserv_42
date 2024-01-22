#ifndef CGI_HPP
# define CGI_HPP

# include <map>
# include <unistd.h>
# include "Request.hpp"
# include "config/Config.hpp"
# include "Webserv.hpp"
# include "Response.hpp"
# define env_iter std::map<std::string, std::string>::const_iterator

/* refer to rfc3875 */
class CgiHandler {
    public:
        CgiHandler();
        CgiHandler(Request& request, Location& loc_ptr, const std::string& host);
        CgiHandler(CgiHandler const &cgi);
        ~CgiHandler();

        CgiHandler &operator=(CgiHandler const &cgi);

        void                                execCgi(Request &req, const Location& loc_ptr);
        const std::string&                  getBody();
        void                                setBody(const std::string& ebody);
        void                                printEnv();

        /*setter & getter*/
        void                                setCgiPath(const std::string& cgi_path);
        const std::string&                  getCgiPath() const;

        int                                 pipe_in[2]; //to get input body throw by response
        int                                 pipe_out[2]; //to send output from cgi to build response

    private:
        std::map<std::string, std::string>  _env;
        std::string                         _body;
        std::string                         _cgiPath;
        char**                              _args;

        std::string                         _getPathInfo(const std::string& req_path, const std::vector<std::string>& exts);
        std::string                         _decodeQuery(std::string req_query);
        std::string                         _getScriptFilename(const std::string& cgi_path);
        void                                _initEnv(Request& request, Location& loc_ptr, const std::string& host);
        void                                _setEnv(Request& request, Location& loc_ptr, const std::string& host);
        char**                              _envToCstrArr() const;
};


#endif