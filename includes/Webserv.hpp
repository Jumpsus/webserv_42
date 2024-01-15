#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# ifdef __linux__
#  define OS    "linux"
# elif BSD
#  define OS    "BSD"
# else
#  define OS    "other"
# endif

/*socket related library*/
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <cerrno>
# include <cstring>
# include <stdlib.h>
# include <fcntl.h>

#endif