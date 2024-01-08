SRCS =	srcs/utils.cpp \
		srcs/Request.cpp \
		srcs/Config.cpp	\
		srcs/Server.cpp \
		srcs/Cgi.cpp \
		srcs/Location.cpp \
		

all:
	c++ -std=c++98 main.cpp ${SRCS} -I includes -o webserv

clean:
	rm webserv

re: clean all