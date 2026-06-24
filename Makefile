NAME = server

SRC	=	main.cpp \
		HttpParser.cpp \
		Configparsing/AWebservParser.cpp \
		Configparsing/WebservCoreParser.cpp \
		Configparsing/WebservCoreMerger.cpp \
		Configparsing/WebservCoreModule.cpp \
		Configparsing/WebservIndexParser.cpp \
		Configparsing/WebservIndexMerger.cpp \
		Configparsing/WebservIndexModule.cpp \
		Configparsing/WebservRedirectParser.cpp \
		Configparsing/WebservRedirectMerger.cpp \
		Configparsing/WebservRedirectModule.cpp \
		Configparsing/ConfigParser.cpp \
		Configparsing/modules.cpp \
		HttpException.cpp \
		Response.cpp \
		Utils.cpp \
		Method.cpp \
		Epoller.cpp \
		AEventHandler.cpp \
		Listener.cpp \
		HeadReader.cpp \
		BodyReader.cpp \
		Writer.cpp \
		Executor.cpp \
		WebServ.cpp


OBJ_DIR = obj

OBJ := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

vpath %.cpp src

CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -std=c++17 -Iinc

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) && echo "\033[32mCompilation successful!\033[0m"

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