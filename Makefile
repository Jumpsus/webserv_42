SRCS =	srcs/utils.cpp \
		srcs/Request.cpp \
		srcs/Config.cpp	\
		srcs/Server.cpp \
		srcs/Cgi.cpp \
		# srcs/ConfigFile.cpp \
		# srcs/Location.cpp \
		

all:
	c++ main.cpp ${SRCS} -I includes -o webserv