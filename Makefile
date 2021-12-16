CC = g++

.PHONY: all
all : ../test/hw3

OBJS = hw3.o user.o database.o server.o command2Enum.o

HEADERS = user.hpp database.hpp server.hpp command2Enum.hpp board.hpp

CPPFILES = hw3.cpp user.cpp database.cpp server.cpp command2Enum.cpp

../test/hw3: $(OBJS)
	$(CC) -o $@ $^

$(OBJS): $(CPPFILES) $(HEADERS)
	$(CC) -c -w $(CPPFILES)

.PHONY: clean
clean:
	rm -rf *.o *.gch hw3
