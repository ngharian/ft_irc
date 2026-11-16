NAME = ircserv

SRC =	./src/main.cpp \
		./src/Server.cpp \
		./src/Server_client_gestion.cpp \
		./src/Server_parsing.cpp \
		./src/Server_free.cpp \
		./src/Client.cpp \
		./src/Channel.cpp \
		./src/commands/Join.cpp \
		./src/commands/Nick.cpp \
		./src/commands/Pass.cpp \
		./src/commands/Ping.cpp \
		./src/commands/Privmsg.cpp \
		./src/commands/User.cpp \
		./src/commands/Cap.cpp	\
		./src/commands/Quit.cpp \
		./src/commands/Part.cpp \
		./src/commands/Kick.cpp \
		./src/commands/Topic.cpp \
		./src/commands/Mode.cpp	\
		./src/commands/Invite.cpp \
		./src/commands/Who.cpp 

INCL = ./incl

OBJ = ${SRC:%.cpp=%.o}

CXX = c++

CPPFLAGS = -Wall -Werror -Wextra -std=c++98

all : ${NAME}

${NAME} : ${OBJ}
	${CXX} ${CPPFLAGS} -o ${NAME} ${OBJ} -I ${INCL} -lstdc++

clean : 
	rm -rf ${OBJ}

fclean : clean 
	rm -f ${NAME}

re : fclean all

.PHONY : all clean fclean re
