#ifndef UTILS_HPP
# define UTILS_HPP

# include <utility>
# include <string>
# include <sstream>
# include <ostream>
# include <vector>
# include <cctype>
# include <iostream>
# include <fstream>
# include <string>

# define s_iter std::vector<std::string>::iterator

/* utils dor request */
std::string                         splitString(std::string &str, std::string delimeter);
std::pair<std::string, std::string> parseHeader(std::string req);
bool                                validateURI(std::string uri);
void                                trimPrefixSuffixConfig(std::string &input);
std::string                         findBlock(std::string src, const std::string& blockName, bool trim);
size_t                              shiftBlock(const std::string& src, const std::string& blockName);
std::string                         findNextWord(std::string src);
bool                                readFile(std::string file_location, std::string& buffer);

bool                                ft_isdigit(const std::string& input);
bool                                ft_ishex(std::string input);
int                                 ft_stoi(const std::string& input);
int                                 ft_htoi(std::string input);
bool                                ft_is_valid_host(const std::string& input);
unsigned long                       ft_pton(std::string host);
std::string                         ft_toupper(std::string input);
std::string                         ft_tolower(std::string input);
std::string                         ft_to_string(size_t num);
bool                                ft_is_white_space(char c);
std::string                         ft_trimspace(std::string input);

#endif