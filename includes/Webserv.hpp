#ifndef WEBSERV_HPP
# define WEBSERV_HPP

/*related to Server Manager*/
# define MESSAGE_BUFFER 40000

/*system related library*/
# include <sys/time.h>
# include <sys/wait.h>
# include <unistd.h>
# include <cstring>
# include <stdlib.h>
# include <fcntl.h>

/*socket related library*/
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cerrno>


#endif