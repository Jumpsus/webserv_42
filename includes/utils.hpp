#ifndef UTILS_HPP
# define UTILS_HPP

# include <utility>
# include <string>
# include <sstream>
# include <ostream>
# include <vector>

/* utils dor request */
std::string                         splitString(std::string &str, std::string delimeter);
std::pair<std::string, std::string> parseHeader(std::string req);
bool                                validateURI(std::string uri);
void                                trimPrefixSuffixConfig(std::string &input);
bool                                removeBracket(std::string &src, std::string prefix);
bool                                getBlock(std::string &src, std::string &des, std::string blockName);
std::vector<std::string>            splitConfig(std::string line);

bool                                ft_isdigit(std::string input);
int                                 ft_stoi(std::string input);
std::string                         ft_toupper(std::string input);
std::string                         ft_to_string(size_t num);

#endif