SRCS =	srcs/utils.cpp \
		srcs/Request.cpp \
		srcs/Response.cpp \
		srcs/Config.cpp	\
		srcs/Server.cpp \
		srcs/Cgi.cpp \
		srcs/Location.cpp \
		srcs/ServerManager.cpp \
		srcs/Client.cpp

all:
	c++ -std=c++98 main.cpp ${SRCS} -I includes -o webserv

clean:
	rm webserv

re: clean all