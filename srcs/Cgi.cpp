#include "Cgi.hpp"

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
1) https://www6.uniovi.es/~antonio/ncsa_httpd/cgi/env.html
2) https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
*/

void CgiHandler::_setEnv(Request& request, Location& loc_ptr)
{
    std::map<std::string, std::string> headers = request.getHeader();
    /* The name of the authentication scheme used to protect the servlet. 
       If the "Auth-Scheme" is found & not empty, it set "AUTH_TYPE*/
    if (headers.find("Auth-Scheme") != headers.end() && headers["Auth-Scheme"] != "")
        this->_env["AUTH_TYPE"] = headers["Authorization"];

    /* The following environment variables are not request-specific 
        and are set for all requests */
    this->_env["SERVER_SOFTWARE"] = "WebServ/1.0";
    if (headers.find("Hostname") != headers.end())
        this->_env["SERVER_NAME"] = headers["Hostname"];
    else
        this->_env["SERVER_NAME"] = "";
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";

    /* The following environment variables are specific to 
       the request being fulfilled by the gateway program */

    //this->_env["SERVER_PROTOCOL"] = request.getVersion();

    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1"; //Supposed to be HTTP 1.1 for HTTP servlets
    this->_env["SERVER_PORT"] = request.getPort(); //TODO
    this->_env["REQUEST_METHOD"] = request.getMethod();
    this->_env["PATH_INFO"] = request.getPath();
    this->_env["PATH_TRANSLATED"] = request.getPath();
    this->_env["SCRIPT_NAME"] = "cgi-bin/" + loc_ptr.getPath()[0];
    this->_env["QUERY_STRING"] = request.getQuery();

    /* The following environment variables are related to the machine that made a reqeust*/
    this->_env["REMOTE_HOST"] = request.getClientHostname();
    this->_env["REMOTE_ADDR"] = request.getClientIP();
    if (this->_env["SERVER_NAME"] == "")
        this->_env["SERVER_NAME"] = this->_env["REMOTE_ADDR"];
    this->_env["REMOTE_USER"] = headers["Authorization"];
    this->_env["REMOTE_IDENT"] = headers["Authorization"];

    this->_env["CONTENT_TYPE"] = headers["Content-Type"];
    this->_env["CONTENT_LENGTH"] = ft_to_string(request.getBody().length());
    
    this->_env["HTTP_ACCEPT"] = "HTTP_" + this->_env["CONTENT_TYPE"];
    this->_env["HTTP_USER_AGENT"] = "HTTP_" + this->_env["USER-AGENT"];
    this->_env["HTTP_COOKIE"] = headers["cookie"];

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
    for (env_iter it = this->_env.begin() ; env_iter != this->_env.end() ; it++)
    {
        std::string     elem = it->first + "=" + it->second;
        c_env[j] = new char[elem.size() + 1];
        c_env[j] = strcpy(c_env[j], (const char *)elem.c_str());
        j++;
    }
    return c_env;
}

std::string CgiHandler::execCgi(const std::string& script)
{
    pid_t   pid;
    int     buf_stdin;
    int     buf_stdout;
    char    **env;
    std::string retBody;

    try {
        env = this->_envToCstrArr();
    }
    catch (std::bad_alloc &e) {
        std::cerr << e.what() << std::endl;
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
        return ("Status: 500 \r\n\r\n");
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