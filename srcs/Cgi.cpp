#include "Cgi.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(Request& request, Location& loc_ptr)
{
    this->_body = request.getBody();
    this->_setEnv(request, loc_ptr);
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
    this->_env.clear();
    this->_body.clear();
}

/*_env variables sources 
1) https://linux.die.net/man/5/cgi
2) https://docs.fileformat.com/th/executable/cgi/#google_vignette (thai)
*/

void CgiHandler::_setEnv(Request& request, Location& loc_ptr)
{
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
    this->_env["REMOTE_ADDR"] = headers["host"]; //currently consider adding DNS search
    this->_env["SERVER_NAME"] = splitString(headers["host"], ":");
    this->_env["SERVER_PORT"] = headers["host"].substr(this->_env["SERVER_NAME"].length() + 1, headers["host"].length());
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["SERVER_SOFTWARE"] = "NGINX";
    headers.clear();
    this->_body = request.getBody();
}

void CgiHandler::setBody(const std::string& ebody)
{
    this->_body = ebody;
}

char**  CgiHandler::_envToCstrArr() const
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

std::string CgiHandler::execCgi(const std::string& script, int &error)
{
    pid_t   pid;
    int     buf_stdin;
    int     buf_stdout;
    char    **env;
    std::string retBody;

    env = this->_envToCstrArr();
    if (env[0] == NULL || env[1] == NULL)
    {
        error = 500;
        return "";
    }
    buf_stdin = dup(STDIN_FILENO);
    buf_stdout = dup(STDOUT_FILENO);

    FILE    *fIn = tmpfile();
    FILE    *fOut = tmpfile();
    long    fdIn = fileno(fIn);
    long    fdOut = fileno(fOut);
    int     ret = 1;

    write(fdIn, _body.c_str(), _body.size());
    lseek(fdIn, 0, SEEK_SET);

    pid = fork();
    if (pid == -1)
        error = 500;
    else if (!pid)
    {
        dup2(fdIn, STDIN_FILENO);
        dup2(fdOut, STDOUT_FILENO);
        execve(script.c_str(), NULL, env);
        write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
    }
    else
    {
        char    buffer[CGI_BUFSIZE] = {0};

        waitpid(-1, NULL, 0);
        lseek(fdOut, 0, SEEK_SET);
        ret = 1;
        while (ret > 0)
        {
            memset(buffer, 0, CGI_BUFSIZE);
            ret = read(fdOut, CGI_BUFSIZE - 1);
            retBody += buffer;
        }
        dup2(buf_stdin, STDIN_FILENO);
        dup2(buf_stdout, STDOUT_FILENO);
        fclose(fIn);
        fclose(fOut);
        close(fdIn);
        close(fdOut);
        close(buf_stdin);
        close(buf_stdout);
        for (size_t i = 0; env[i]; i++)
            delete[] env[i];
        delete[] env;

        if (!pid)
            exit(0);
        return (retBody);
    }
}

/*This function check if PATH_INFO exist it request path, by finding the occurance of cgi extentions*/
std::string CgiHandler::_getPathInfo(const std::string& req_path, const std::vector<std::string>& exts)
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
std::string CgiHandler::_decodeQuery(std::string req_query)
{
    size_t  p_pos = req_query.find("%");
    while (p_pos != std::string::npos)
    {
        if (req_query.length() < p_pos + 2)
            break ;
        char    r_char = ft_htoi(req_query.substr(p_pos + 1, 2));
        req_query.replace(p_pos, 3, std::to_string(r_char));
        p_pos = req_query.find("%");
    }
    return req_query;
}