NAME		= clone

CXX			= clang++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++20 -Iinc
DEBUGFLAGS	= -DDEBUG -g3 -fsanitize=address -fsanitize=leak

SRC_DIR		= src
INC_DIR		= inc
OBJ_DIR		= obj

SRCS		= $(wildcard $(SRC_DIR)/*.cpp)
OBJS		= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: fclean all

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug

