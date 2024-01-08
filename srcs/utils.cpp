#include "utils.hpp"
#include <iostream>

std::pair<std::string, std::string> parseHeader(std::string req)
{
    std::size_t found;
    std::string first;
    std::string second;

    found = req.find(':');

    if (found != std::string::npos)
    {
        first = req.substr(0, found);

        if (req.length() != found)
        {
            second = req.substr(found + 1, req.length());
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

    for (int i = 0; i < uri.length(); i++)
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

bool    ft_isdigit(std::string input)
{
    for (int i = 0; i < input.length(); i++)
    {
        if (input[i] < '0' || input[i] > '9')
        {
            return false;
        }
    }
    
    return true;
}

int     ft_stoi(std::string input)
{
    int     result = 0;

    if (!ft_isdigit(input))
    {
        return 0;
    }

    for (int i = 0; i < input.length(); i++)
    {
        result = result * 10 + input[i] - '0';
    }
    return result;
}

bool    ft_is_valid_host(std::string input)
{
    std::string travel = input;
    std::string tempString = "";
    int         tempInt = 0;

    for (int i = 0; i < 3; i++)
    {
        tempString = splitString(travel, ".");
        if (tempString == "" || !ft_isdigit(tempString))
        {
            return false;
        }

        tempInt = ft_stoi(tempString);
        if (tempInt > 255 || tempInt < 0)
        {
            return false;
        }
    }

    if (travel == "" || !ft_isdigit(travel))
    {
        return false;
    }

    tempInt = ft_stoi(travel);
    if (tempInt > 255 || tempInt < 0)
    {
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

    if (!ft_is_valid_host(host))
    {
        return 0;
    }

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

    for (int i = 0; i < res.length(); i++)
    {
        if (res[i] >= 'a' && res[i] <= 'z')
        {
            res[i] = res[i] + offset;
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
    int i = 0;
    int j = input.length();

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
std::string    findBlock(std::string src, std::string blockName)
{
    int             count = 0;
    size_t          index = 0;
    std::string     des;

    index = src.find(blockName);

    /* first word isn't "server" */
    if (index != 0)
    {
        return des;
    }

    index += blockName.length();
    /* find first bracket '{' */
    while (index < src.length())
    {
        if (src[index] == '{')
        {
            count++;
        }
        index++;

        if (count > 0)
        {
            break;
        }
    }

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
        des = src.substr(0, index + 1);
    }

    return des;
}

std::string findNextWord(std::string src)
{
    int         index = 0;
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

bool removeBracket(std::string &src, std::string prefix)
{
    size_t  found;
    int     index;

    trimPrefixSuffixConfig(src);
    found = src.find(prefix);
    if (found != 0)
    {
        return false;
    }

    if (src.length() == 0 || src[src.length() - 1] != '}')
    {
        return false;
    }

    index = prefix.length();

    while (index < src.length())
    {
        switch (src[index])
        {
            case ' ':
                index++;
                break;
            case '\t':
                index++;
                break;
            case '\v':
                index++;
                break;
            case '\n':
                index++;
                break;
            case '\r':
                index++;
                break;
            case '{':
                index++;
                src = src.substr(index, src.length() - 1);
                return true;
            default:
                return false;
        }
    }
    return false;
}
