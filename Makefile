NAME = server

SRC = src/main.cpp \
		src/Eventloop.cpp \
		src/HttpParser.cpp \
		src/Client.cpp

OBJ := $(SRC:%.cpp=%.o)

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -Iinc

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) && echo "\033[32mCompilation successful!\033[31m"

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	@rm -f $(OBJ) && echo "\033[31mObjects cleared!\033[0m"

fclean:
	@rm -f $(NAME) $(OBJ) > /dev/null && echo "\033[31mProgram cleared successfully!\033[0m"

re: fclean all

.PHONY: all fclean re