#include "utils.hpp"
#include <iostream>

std::pair<std::string, std::string> parseHeader(std::string req)
{
    std::size_t found;
    std::string first = req;
    std::string second = "";

    found = req.find(':');

    if (found != std::string::npos)
    {
        first = ft_tolower(req.substr(0, found));

        if (req.length() != found)
        {
            second = ft_trimspace(req.substr(found + 1, req.length()));
        }
    }

    return std::pair<std::string, std::string>(first, second);
}

std::string splitString(std::string &str, std::string delimeter)
{
    std::size_t     found;
    std::string     rtn;

    found = str.find(delimeter);

    if (found != std::string::npos)
    {
        rtn = str.substr(0, found);
        str = str.substr(found + delimeter.length(), str.length());
        return rtn;
    }

    return "" ;
}

bool    validateURI(std::string uri){
    char    c;

    for (size_t i = 0; i < uri.length(); i++)
    {
        c = uri[i];
        if ( c <= ' ' || c == '"' || c == '<' || c == '>' || c == '\\' || 
        c == '^' || c == '`' || c == '{' || c == '}' || c == '|' || c > '~' )
        {
            return false;
        }
    }
    return true;
}

bool    ft_isdigit(const std::string& input) //this declaration is more memory efficient
{
    for (std::string::const_iterator it = input.begin() ; it != input.end(); ++it)
    {
        if (!std::isdigit(*it))
            return false;
    }
    return  true;
}

// accept only lower case
bool    ft_ishex(std::string input)
{
    for (int i = 0; i < input.length(); i++)
    {
        if ((input[i] >= '0' && input[i] <= '9') || 
            (input[i] >= 'a' && input[i] <= 'f'))
        {
            return false;
        }
    }
    
    return true;
}

int     ft_stoi(const std::string& input)
{
    int     result = 0;

    if (!ft_isdigit(input))
    {
        return 0;
    }

    for (size_t i = 0; i < input.length(); i++)
    {
        result = result * 10 + input[i] - '0';
    }
    return result;
}

int     ft_htoi(std::string input)
{
    int     result = 0;
    int     temp_val = 0;

    if (!ft_ishex(input))
    {
        return 0;
    }

    for (int i = 0; i < input.length(); i++)
    {
        if (input[i] >= '0' && input[i] < '9')
        {
            temp_val = input[i] - '0';
        } else if (input[i] >= 'a' && input[i] < 'f') {
            temp_val = input[i] - 'a' + 10;
        }
        result = result * 16 + temp_val;
    }
    return result;
}

bool    ft_is_valid_host(const std::string& input)
{
    std::string travel = input;

    while (1)
    {
        size_t  pos = travel.find(".");
        if (travel.find(".") == std::string::npos) {
            if (travel == "" || !ft_isdigit(travel))
                return (false);
            int tempInt = ft_stoi(travel);
            if (tempInt > 255 || tempInt < 0)
                return false;
            break ;
        }
        std::string token = travel.substr(0, pos);
        travel = travel.substr(pos + 1, travel.length() - (pos + 1));
        if (token == "" || !ft_isdigit(token))
            return false;
        int         token_int = ft_stoi(token);
        if (token_int > 255 || token_int < 0)
            return false;
    }
    return true;
}

unsigned long   ft_pton(std::string host)
{
    std::string     travel = host;
    std::string     tempString = "";
    int             tempInt[3];
    unsigned long   rtn = 0;

    /*if (!ft_is_valid_host(host))
    {
        return 0;
    }*/

    for (int i = 0; i < 3; i++)
    {
        tempString = splitString(travel, ".");
        tempInt[i] = ft_stoi(tempString);
    }

    rtn = ft_stoi(travel);
    for (int i = 2; i >= 0; i--)
    {
        rtn = (rtn * 256) + tempInt[i];
    }

    return rtn;
}

std::string ft_toupper(std::string input)
{
    std::string res = input;
    int offset = 'A' - 'a';

    for (size_t i = 0; i < res.length(); i++)
    {
        if (res[i] >= 'a' && res[i] <= 'z')
        {
            res[i] = res[i] + offset;
        }
    }
    return (res);
}

std::string ft_tolower(std::string input)
{
    std::string res = input;
    int offset = 'A' - 'a';

    for (int i = 0; i < res.length(); i++)
    {
        if (res[i] >= 'A' && res[i] <= 'Z')
        {
            res[i] = res[i] - offset;
        }
    }
    return (res);
}

std::string ft_to_string(size_t num)
{
    std::ostringstream str1;

    str1 << num;
    return (str1.str());
}

bool ft_is_white_space(char c)
{
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v')
    {
        return true;
    }

    return false;
}

/* trim prefix white space ,tab ,line feed ,carriage return and verticle tab */
void    trimPrefixSuffixConfig(std::string &input)
{
    size_t i = 0;
    size_t j = input.length();

    while (i < input.length())
    {
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n' || input[i] == '\r' || input[i] == '\v')
        {
            i++;
        } else {
            break;
        }
    }

    while (j > 0)
    {
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n' || input[i] == '\r' || input[i] == '\v')
        {
            j--;
        } else {
            break;
        }   
    }
    input = input.substr(i, j);
    return;
}

/* return true when success, return false when fail */
std::string    findBlock(std::string src, const std::string& blockName, bool trim)
{
    int             count = 0;
    size_t          index = 0, start;
    std::string     des;


    index = src.find(blockName);

    /* return zero if blockName (e.g. server) not exist in the src */
    if (index == std::string::npos)
    {
        return des;
    }

    index += blockName.length();
    /* find first bracket '{' */
    while (index < src.length())
    {
        if (src[index] == '{')
        {
            trim == true? start = index : start = 0;
            count++;
        }
        index++;

        if (count > 0)
        {
            break;
        }
    }
    //start = index;
    while (index < src.length() && count > 0)
    {
        switch (src[index])
        {
            case '{':
                index++;
                count++;
                break;
            case '}':
                index++;
                count--;
                break;
            default:
                index++;
        }
    }
    if (count == 0)
    {
        des = src.substr(start, index - start);
    }

    return des;
}

std::string findNextWord(std::string src)
{
    size_t      index = 0;
    std::string des;

    /* delimeter in config nginx */
    if (src[0] == '{' || src[0] == '}' || src[0] == ';')
    {
        return (src.substr(0, 1));
    }

    while (index < src.length())
    {
        if (ft_is_white_space(src[index]))
        {
            return (src.substr(0, index));
        }

        /* delimeter in config nginx */
        switch (src[index])
        {
            case '{':
                return (src.substr(0, index));
            case '}':
                return (src.substr(0, index));
            case ';':
                return (src.substr(0, index));
        }
        index++;
    }

    return (src.substr(0, index));
}

std::string ft_trimspace(std::string input)
{
    int i = 0;

    while (input[i] == ' ')
    {
        i++;
    }
    return (input.substr(i, input.length()));
}

size_t  shiftBlock(const std::string& src, const std::string& blockName)
{
    size_t  ret = src.find(blockName);

    if (ret == std::string::npos)
        return 0;
    ret += blockName.length();
    while (ret < src.length() && src[ret] != '{')
        ret++;
    return ret;
}

bool     readFile(std::string file_location, std::string& buffer)
{
    std::ifstream file(file_location.c_str());
    std::string    temp_string;

    buffer = "";

    if (file.is_open())
    {
        while (file)
        {
            std::getline(file, temp_string);
            buffer.append(temp_string);
            buffer.append("\n");
            temp_string = "";
        }
        file.close();
    } else {
        return (false);
    }

    return (true);
}

bool    ft_isURI(const std::string& word)
{
    /* : / ? # [ ] @ ! $ & ' ( ) * + , ; = */
    /*all alphabet, number, - , . , _ , ~*/
    for (int i = 0; i < word.length(); i++)
    {
        if ( !((word[i] >= '#' && word[i] <= ';') || (word[i] >= '?' && word[i] <= '[') || (word[i] >= 'a' && word[i] <= 'z') ||
                word[i] == '!' || word[i] == '=' || word[i] == ']' || word[i] == '_' || word[i] == '~') )
            return (false);
    }
    return (true);
}

std::string ft_join(std::string path1, std::string path2)
{
    if (path1[path1.length() - 1] == '/' && path2[0] == '/')
    {
        return (path1.substr(0, path1.length() - 1) + path2);
    }

    if (path1[path1.length() - 1] != '/' && path2[0] != '/')
    {
        return (path1 + "/" + path2);
    }

    return (path1 + path2);
}

std::string getExtension(std::string file)
{
    std::string ext = "";
    
    for (int i = file.length() - 1; i >= 0 ; i--)
    {
        if (file[i] == '.')
        {
            return (ext);
        }
        ext = file[i] + ext;
    }

    return "";
}

std::string mapStatusCode(int status)
{
    switch (status)
    {
        case 100:
            return("Continue");
        case 101:
            return("Switching Protocols");
        case 200:
            return("OK");
        case 201:
            return("Created");
        case 202:
            return("Accepted");
        case 203:
            return("Non-Authoritative Information");
        case 204:
            return("No Content");
        case 205:
            return("Reset Content");
        case 206:
            return("Partial Content");
        case 300:
            return("Multiple Choices");
        case 301:
            return("Moved Permanently");
        case 302:
            return("Found");
        case 303:
            return("See Other");
        case 304:
            return("Not Modified");
        case 305:
            return("Use Proxy");
        case 307:
            return("Temporary Redirect");
        case 400:
            return("Bad Request");
        case 401:
            return("Unauthorized");
        case 402:
            return("Payment Required");
        case 403:
            return("Forbidden");
        case 404:
            return("Not Found");
        case 405:
            return("Method Not Allowed");
        case 406:
            return("Not Acceptable");
        case 407:
            return("Proxy Authentication Required");
        case 408:
            return("Request Timeout");
        case 409:
            return("Conflict");
        case 410:
            return("Gone");
        case 411:
            return("Length Required");
        case 412:
            return("Precondition Failed");
        case 413:
            return("Payload Too Large");
        case 414:
            return("URI Too Long");
        case 415:
            return("Unsupported Media Type");
        case 416:
            return("Range Not Satisfiable");
        case 417:
            return("Expectation Failed");
        case 426:
            return("Upgrade Required");
        case 500:
            return("Internal Server Error");
        case 501:
            return("Not Implemented");
        case 502:
            return("Bad Gateway");
        case 503:
            return("Service Unavailable");
        case 504:
            return("Gateway Timeout");
        case 505:
            return("HTTP Version Not Supported");
    }

    return("UNKNOWN");
}

std::string     mapContentType(std::string file)
{
    std::string extension = getExtension(file);

    if (extension == "txt") 
        return ("text/plain");
    if (extension == "htm" || extension == "html") 
        return ("text/html");
    if (extension == "css") 
        return ("text/css");
    if (extension == "csv") 
        return ("text/csv");
    if (extension == "js") 
        return ("text/javascript");
    if (extension == "apng") 
        return ("image/apng");
    if (extension == "avif") 
        return ("image/avif");
    if (extension == "gif") 
        return ("image/gif");
    if (extension == "jpg" || extension == "jpeg") 
        return ("image/jpeg");
    if (extension == "png") 
        return ("image/png");
    if (extension == "svg") 
        return ("image/svg+xml");
    if (extension == "webp") 
        return ("image/webp");
    if (extension == "ico") 
        return ("image/vnd.microsoft.icon");
    if (extension == "avi") 
        return ("video/x-msvideo");
    if (extension == "bin") 
        return ("application/octet-stream");
    if (extension == "json") 
        return ("application/json");
    if (extension == "gz") 
        return ("application/gzip");
    if (extension == "doc")	     
        return ("application/msword");
    if (extension == "php")	     
        return ("application/x-httpd-php");
    if (extension == "pdf")	     
        return ("application/pdf");
    if (extension == "json")	     
        return ("application/json");
    if (extension == "xml")	     
        return ("application/xml");
    if (extension == "mp3") 
        return ("audio/mpeg");
    if (extension == "mp4") 
        return ("audio/mp4");
    if (extension == "webm") 
        return ("audio/webm");
    if (extension == "ttf") 
        return ("font/ttf");
    if (extension == "woof") 
        return ("font/woof");
    if (extension == "woof2") 
        return ("font/woof2");
        
    return ("text/plain");
}

bool    isFileExists(std::string file_location)
{
    std::ifstream file(file_location.c_str());
    return (file.good());
}

bool    isDirectory(std::string file_location)
{
    struct stat s;

    if( stat(file_location.c_str(), &s) == 0 )
    {
        return ( s.st_mode & S_IFDIR );
    }

    return (false);
}

std::string defaultErrorPage(int error)
{
    std::string errorPage;

    errorPage = "<html>\r\n<head>\r\n<title>" + ft_to_string(error) +
                " " + mapStatusCode(error) + "</title>\r\n</head>\r\n" +
                "<body>\r\n<h1>" + ft_to_string(error) + " " + 
                mapStatusCode(error)+ "</h1>\r\n</body>\r\n</html>" ;

    return (errorPage);
}

std::string ft_find_by_keyword(std::vector<std::string> vec, const std::string& needle)
{
    for (s_iter it = vec.begin(); it != vec.end(); it++)
    {
        std::string iter_buff = *it;
        if (iter_buff.find(needle) != std::string::npos)
            return iter_buff;
    }
    return "";
}