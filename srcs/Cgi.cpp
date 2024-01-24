#include "Cgi.hpp"

CgiHandler::CgiHandler() 
{
    this->_body = "";
    this->_args0 = "";
    this->_cgiPath = "";
}

CgiHandler::CgiHandler(Request& request, Location& loc_ptr)
{
    this->_body = request.getBody();
    this->setEnv(request, loc_ptr);
    this->_args0 = "";
    this->_cgiPath = "";
}

CgiHandler::CgiHandler(CgiHandler const &cgi)
{
    if (this != &cgi)
    {
        this->_env = cgi._env;
        this->_body = cgi._body;
        this->_args0 = cgi._args0;
        this->_cgiPath = cgi._cgiPath;
    }
}

CgiHandler &CgiHandler::operator=(CgiHandler const &cgi)
{
    if (this != &cgi)
    {
        this->_env = cgi._env;
        this->_body = cgi._body;
        this->_args0 = cgi._args0;
        this->_cgiPath = cgi._cgiPath;
    }

    return (*this);
}

CgiHandler::~CgiHandler()
{
    this->_env.clear();
    this->_body.clear();
}

void                CgiHandler::setCgiPath(const std::string& cgi_path)
{
    this->_cgiPath = cgi_path;
}

const std::string&  CgiHandler::getCgiPath() const
{
    return this->_cgiPath;
}

pid_t               CgiHandler::getCgiPid()
{
    return this->_cgi_pid;
}

void                CgiHandler::setArgs0(const std::string& extension, const Location& loc_ptr)
{
    s_iter path_ptr;

    if (extension == ".py") {
        path_ptr = ft_find_by_keyword(loc_ptr.getCgiPath(), "python");
        if (path_ptr != loc_ptr.getCgiPath().end())
            this->_args0 = *path_ptr;
    }
    if (extension == ".sh") {
        path_ptr = ft_find_by_keyword(loc_ptr.getCgiPath(), "bash");
        if (path_ptr != loc_ptr.getCgiPath().end())
            this->_args0 = *path_ptr;
    }
}

std::string         CgiHandler::_getScriptFilename(const std::string& cgi_path)
{
    int pos = cgi_path.find("cgi-bin/");

    if (pos == std::string::npos)
        return "";
    if (pos + 8 > (int)cgi_path.size())
        return "";
    return cgi_path.substr(pos + 8, cgi_path.size());
}

std::string         CgiHandler::_getRemoteAddress(const std::string& http_host)
{
    std::string hostname = http_host.substr(0, http_host.length() - http_host.find(':'));
    if (hostname.length() == 0)
        return "";
    if (hostname == "localhost")
        return "127.0.0.1";
    return hostname;
}

/*_env variables sources 
1) https://linux.die.net/man/5/cgi
2) https://docs.fileformat.com/th/executable/cgi/#google_vignette (thai)
*/

void                CgiHandler::setEnv(Request& request, Location& loc_ptr)
{
   /*get CGI path & extensions
    1) check if the path's extension is exist in the location setting
    2) check if the path's extension have matching folder
    3) parse the folder to args[0]
    4) parse the path to args[1]
    */
    if (this->_args0 == "" || this->_cgiPath == "")
        return ;
    this->_args[0] = strdup(this->_args0.c_str());
    this->_args[1] = strdup(this->_cgiPath.c_str());
    this->_args[2] = NULL;
    
    /*set env variables per reqeust file*/
    std::map<std::string, std::string> headers = request.getHeader();
    if (headers.find("Auth-Scheme") != headers.end() && headers["Auth-Scheme"] != "")
        this->_env["AUTH_TYPE"] = headers["authorization"];

    this->_env["CONTENT_LENGTH"] = headers["content-length"];
    this->_env["CONTENT_TYPE"] = headers["content-type"];
    this->_env["DOCUMENT_ROOT"] = loc_ptr.getRoot();
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->_env["HTTP_ACCEPT"] = headers["accept"];
    this->_env["HTTP_ACCEPT_ENCODING"] = headers["accept-encoding"];
    this->_env["HTTP_ACCEPT_LANGUAGE"] = headers["accept-language"];
    this->_env["HTTP_CONNECTION"] = headers["connection"];
    this->_env["HTTP_HOST"] = headers["host"];
    this->_env["HTTP_REFERER"] = headers["referer"];
    this->_env["HTTP_USER_AGENT"] = headers["user-agent"];
    this->_env["PATH_INFO"] = _getPathInfo(request.getPath(), loc_ptr.getCgiExt());
    this->_env["PATH_TRANSLATED"] = loc_ptr.getRoot() + (this->_env["PATH_INFO"] == "" ? "/" : this->_env["PATH_INFO"]);
    this->_env["QUERY_STRING"] = _decodeQuery(request.getQuery());

    /*set basic CGI variable*/
    this->_env["SCRIPT_NAME"] = this->_cgiPath;
    this->_env["SCRIPT_FILENAME"] = _getScriptFilename(this->_cgiPath);
    this->_env["REMOTE_ADDR"] = _getRemoteAddress(headers["host"]);
    this->_env["SERVER_NAME"] = splitString(headers["host"], ":");
    this->_env["SERVER_PORT"] = headers["host"].substr(this->_env["SERVER_NAME"].length() + 1, headers["host"].length());
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["SERVER_SOFTWARE"] = "WEBSERV";
    headers.clear();
    this->_body = request.getBody();
}

void                CgiHandler::setBody(const std::string& ebody)
{
    this->_body = ebody;
}

char**              CgiHandler::_envToCstrArr() const
{
    char    **c_env = new char*[this->_env.size() + 1];
    int     j = 0;

    c_env[this->_env.size()] == NULL;
    for (env_iter it = this->_env.begin() ; it != this->_env.end() ; it++)
    {
        std::string     elem = it->first + "=" + it->second;
        c_env[j] = new char[elem.size() + 1];
        c_env[j] = strcpy(c_env[j], (const char *)elem.c_str());
        j++;
    }
    return c_env;
}

void                CgiHandler::execCgi(Request &req)
{
    char    **cgi_env = _envToCstrArr();

    //can't set _args
    if (this->_args[0] == NULL || this->_args[1] == NULL) {
        std::cerr << "Can't initiate args\n";
        req.setError(500);
        return ;
    }
    //can't set cgi_env
    if (cgi_env[0] == NULL || cgi_env[1] == NULL) {
        std::cerr << "Fail to create cgi env\n";
        req.setError(500);
        return ;
    }
    //can't pipe pipe_in
    if (pipe(pipe_in) < 0) {
        std::cerr << "Fail to create pipe_in\n";
        req.setError(500);
        return ;
    }
    //can't pipe pipe_out
    if (pipe(pipe_out) < 0) {
        std::cerr << "Fail to create pipe_out\n";
        close(pipe_in[0]);
        close(pipe_in[1]);
        req.setError(500);
        return ;
    }
    //start fork process
    _cgi_pid = fork();
    if (_cgi_pid < 0) {
        std::cerr << "Fail to fork\n";
        
        req.setError(500);
        return ;
    }
    //successful fork
    if (_cgi_pid == 0) {
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_in[0]);
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(pipe_out[1]);
        if (execve(_args[0], _args, cgi_env)) {
            std::cerr << "Fail to exec cgi script\n";
            req.setError(500);
            return ;
        }
    }
}

/*This function check if PATH_INFO exist it request path, by finding the occurance of cgi extentions*/
std::string             CgiHandler::_getPathInfo(const std::string& req_path, const std::vector<std::string>& exts)
{
    std::string temp_str;
    size_t  begin, end;

    /*find if request "_path" contain "_cgi_ext"*/
    for (std::vector<std::string>::const_iterator ext_ptr = exts.begin(); ext_ptr != exts.end(); ext_ptr++)
    {
        if (begin = req_path.find(*ext_ptr) != std::string::npos)
        {
            if (begin + 3 >= req_path.size())
                return "";
            temp_str = req_path.substr(begin + 3, req_path.size());
            if (!temp_str[0] || temp_str[0] != '/')
                return "";
            end = temp_str.find("?");
            if (end == std::string::npos)
                return temp_str;
            else
                return temp_str.substr(0, end);
        }
    }
    return "";
}

/*This function change the "encoded" reserve characters back to the reserve characters
https://en.wikipedia.org/wiki/Percent-encoding --> reserve character*/
std::string             CgiHandler::_decodeQuery(std::string req_query)
{
    size_t  p_pos = req_query.find("%");
    while (p_pos != std::string::npos)
    {
        if (req_query.length() < p_pos + 2)
            break ;
        char    r_char = ft_htoi(req_query.substr(p_pos + 1, 2));
        req_query.replace(p_pos, 3, ft_to_string(r_char));
        p_pos = req_query.find("%");
    }
    return req_query;
}