CXX = gcc
CXXFLAGS = -Wall -g 
SRCS = smallsh.c builtins.c command.c
HEADERS = builtins.h command.h
OBJS = smallsh.o builtins.o command.o

smallish: ${OBJS} ${HEADERS}
	${CXX} ${OBJS} ${HEADERS} -o smallsh


${OBJS}: ${SRCS}
	${CXX} ${CXXFLAGS} ${SRCS} -c ${@:.o=.c}

clean:
	rm -f *.o smallsh
