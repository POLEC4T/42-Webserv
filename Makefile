NAME = webserv

SRC_DIR = src
INC_DIR = includes
SRCS = main.cpp \
	$(SRC_DIR)/APage.cpp \
	$(SRC_DIR)/ErrorPage.cpp \
	$(SRC_DIR)/Location.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/AHttpMethod.cpp
OBJS = $(SRCS:.cpp=.o)
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)
RM = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
