NAME = webserv
FLAGS = -std=c++98 -Wall -Wextra -Werror -g3 -fsanitize=address
INCS = -I includes/
SRCS =	srcs/utils.cpp \
		srcs/Request.cpp \
		srcs/Response.cpp \
		srcs/Config.cpp	\
		srcs/Server.cpp \
		srcs/Location.cpp \
		srcs/ServerManager.cpp \
		srcs/Client.cpp \
		srcs/Cgi.cpp \
		main.cpp

OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(FLAGS) $(INCS) $ -c $< -o $@

all: $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(FLAGS) ${OBJS} $(INCS) -o $(NAME)

clean: $(OBJS)
	rm $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re