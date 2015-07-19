SRC_FILES = main.c
CC_FLAGS = -Wall -Wextra -g 
CC = g++

all:
	${CC} ${SRC_FILES} ${CC_FLAGS} -o basicvm