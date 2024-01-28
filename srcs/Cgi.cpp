#include "Cgi.hpp"

CgiHandler::CgiHandler() 
{
    this->_args0 = "";
    this->_cgiPath = "";
    this->_args = NULL;
    this->_cgi_env = NULL;
}

CgiHandler::CgiHandler(Request& request, Location& loc_ptr)
{
    this->setEnv(request, loc_ptr);
    this->_args0 = "";
    this->_cgiPath = "";
    this->_args = NULL;
    this->_cgi_env = NULL;
}

CgiHandler::CgiHandler(CgiHandler const &cgi)
{
    if (this != &cgi)
    {
        this->_env = cgi._env;
        this->_args0 = cgi._args0;
        this->_cgiPath = cgi._cgiPath;
        this->_cgi_env = cgi._cgi_env;
        this->_args = cgi._args;
    }
}

CgiHandler &CgiHandler::operator=(CgiHandler const &cgi)
{
    if (this != &cgi)
    {
        this->_env = cgi._env;
        this->_args0 = cgi._args0;
        this->_cgiPath = cgi._cgiPath;
        this->_cgi_env = cgi._cgi_env;
        this->_args = cgi._args;
    }

    return (*this);
}

CgiHandler::~CgiHandler()
{
    if (this->_args)
    {
        for (int i = 0; this->_args[i]; i++)
            free(_args[i]);
        free(_args);
    }
    if (this->_cgi_env)
    {
        for (int j = 0; this->_cgi_env[j]; j++)
            free(_cgi_env[j]);
        free(_cgi_env);
    }
    this->_env.clear();
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
    std::string path_str;

    if (extension == ".py") {
        path_str = ft_find_by_keyword(loc_ptr.getCgiPath(), "python");
        if (path_str != "")
            this->_args0 = path_str;
    }
    if (extension == ".sh") {
        path_str = ft_find_by_keyword(loc_ptr.getCgiPath(), "bash");
        if (path_str != "")
            this->_args0 = path_str;
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
    std::string hostname = http_host.substr(0, http_host.find(':'));
    std::string port = http_host.substr(http_host.find(':'));

    if (hostname.length() == 0)
        return "";
    if (hostname == "localhost")
        return "127.0.0.1" + port;
    return hostname + port;
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
    this->_args = (char**)malloc(sizeof(char*) * 3);
    this->_args[0] = strdup(this->_args0.c_str());
    this->_args[1] = strdup(this->_cgiPath.c_str());
    this->_args[2] = NULL;

    /*set env variables per reqeust file*/
    std::map<std::string, std::string> headers = request.getHeader();
    if (headers.find("Auth-Scheme") != headers.end() && headers["Auth-Scheme"] != "")
        this->_env["AUTH_TYPE"] = headers["authorization"];
    else
        this->_env["AUTH_TYPE"] = "Basic";
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
    this->_env["REQUEST_METHOD"] = request.getMethod();
    this->_env["REQUEST_URI"] = request.getPath() + request.getQuery();
    this->_env["REDIRECT_STATUS"] = "200";
    this->_env["SCRIPT_NAME"] = this->_cgiPath;
    this->_env["SCRIPT_FILENAME"] = _getScriptFilename(this->_cgiPath);
    this->_env["REMOTE_ADDR"] = _getRemoteAddress(headers["host"]);
    this->_env["SERVER_NAME"] = splitString(headers["host"], ":");
    this->_env["SERVER_PORT"] = headers["host"];
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["SERVER_SOFTWARE"] = "WEBSERV";
    headers.clear();
}

char**              CgiHandler::_envToCstrArr() const
{
    char    **c_env;
    int     j = 0;

    c_env = (char**)malloc(sizeof(char*) * (this->_env.size() + 1));
    c_env[this->_env.size()] = NULL;
    for (env_iter it = this->_env.begin() ; it != this->_env.end() ; it++)
    {
        std::string     elem = it->first + "=" + it->second;
        c_env[j] = strdup(elem.c_str());
        j++;
    }
    return c_env;
}

bool                CgiHandler::initPipes(int &error)
{
    //can't pipe pipe_in
    if (pipe(pipe_in) < 0) {
        std::cerr << "Fail to create pipe_in\n";
        error = 500;
        return false;
    }
    //can't pipe pipe_out
    if (pipe(pipe_out) < 0) {
        std::cerr << "Fail to create pipe_out\n";
        close(pipe_in[0]);
        close(pipe_in[1]);
        error = 500;
        return false;
    }
    return true;
}

void                CgiHandler::execCgi(int &error, int &status)
{
    //can't set _args
    if (!this->_args || this->_args[0] == NULL || this->_args[1] == NULL) {
        std::cerr << "Can't initiate args\n";
        error = (500);
        return ;
    }

    this->_cgi_env = _envToCstrArr(); //it currently don't working
    //can't set cgi_env
    if (_cgi_env[0] == NULL || _cgi_env[1] == NULL) {
        std::cerr << "Fail to create cgi env\n";
        error = 500;
        return ;
    }
    //start fork process
    _cgi_pid = fork();
    if (_cgi_pid < 0) {
        std::cerr << "Fail to fork\n";
        error = 500;
    }
    //successful fork
    else if (_cgi_pid == 0) {
        //extern char**       environ;
        //for (int i = 0; _args[i]; i++)
        //    std::cout << _args[i] << std::endl;
        //std::cout << std::endl;
        //for (int j = 0; _cgi_env[j]; j++)
        //    std::cout << _cgi_env[j] << std::endl;
        //std::cout << std::endl;
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_in[0]);
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(pipe_out[1]);
        status = execve(_args[0], _args, _cgi_env);
        exit(status);
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
        begin = req_path.find(*ext_ptr);
        if (begin != std::string::npos)
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

void                    CgiHandler::clear()
{
    this->_env.clear();
    this->_args0.clear();
    this->_cgiPath.clear();
}
