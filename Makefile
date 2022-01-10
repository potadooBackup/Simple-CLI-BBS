CC = g++

.PHONY: all
all : main

OBJS = main.o user.o database.o server.o command2Enum.o

HEADERS = ./header/user.hpp ./header/database.hpp ./header/server.hpp ./header/command2Enum.hpp ./header/board.hpp

CPPFILES = main.cpp ./src/user.cpp ./src/database.cpp ./src/server.cpp ./src/command2Enum.cpp

main: $(OBJS)
	$(CC) -o $@ $^

$(OBJS): $(CPPFILES) $(HEADERS)
	$(CC) -c -w $(CPPFILES)

.PHONY: clean
clean:
	rm -rf *.o *.gch main
