NAME = webserv

COLOR_GREEN = \033[0;32m
COLOR_BLUE = \033[0;34m
COLOR_END = \033[0m

SRC_DIR = src
OBJ_DIR = .obj
INC_DIR = includes

SRCS = main.cpp \
	$(SRC_DIR)/APage.cpp \
	$(SRC_DIR)/ErrorPage.cpp \
	$(SRC_DIR)/Location.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/AHttpMethod.cpp
HEADERS = $(INC_DIR)/AHttpMethod.hpp \
	$(INC_DIR)/APage.hpp \
	$(INC_DIR)/CodeDefines.h \
	$(INC_DIR)/ErrorPage.hpp \
	$(INC_DIR)/Location.hpp \
	$(INC_DIR)/Request.hpp \
	$(INC_DIR)/Response.hpp \
	$(INC_DIR)/Server.hpp
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)
RM = rm -rf

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "The program is ready: $(COLOR_GREEN)$(NAME)$(COLOR_END)"

$(OBJ_DIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@echo "$(COLOR_BLUE)Compiling: $<$(COLOR_END)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
