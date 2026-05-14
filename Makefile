NAME = server

SRC = main.cpp \
		HttpServer.cpp \
		HttpParser.cpp \
		Client.cpp \
		Configparsing/ConfigParser.cpp \
		Configparsing/modules.cpp \
		Servers.cpp

OBJ_DIR = obj

OBJ := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

vpath %.cpp src

CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -std=c++17 -Iinc

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) && echo "\033[32mCompilation successful!\033[31m"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	@rm -f $(OBJ) && echo "\033[31mObjects cleared!\033[0m"

fclean:
	@rm -f $(NAME) $(OBJ) > /dev/null && echo "\033[31mProgram cleared successfully!\033[0m"
	@rm -rf $(OBJ_DIR)

re: fclean all

.PHONY: all fclean re