#ifndef WEBSERV_HPP
# define WEBSERV_HPP

/*define color when cout*/
# define RESET          "\x1B[0m"
# define RED            "\x1B[31m"
# define WHITE          "\x1B[37m"
# define YELLOW         "\x1B[33m"
# define BLUE           "\x1B[94m"
# define CYAN           "\x1B[36m"
# define MAGENTA        "\x1B[95m"

/*related to Server Manager*/
# define MESSAGE_BUFFER 40000

/*system related library*/
# include <sys/time.h>
# include <sys/wait.h>
# include <unistd.h>
# include <cstring>
# include <stdlib.h>
# include <fcntl.h>
# include <ctime>

/*socket related library*/
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cerrno>


#endif