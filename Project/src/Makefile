CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++14

SUFFIXES = .cpp .o
.SUFFIXES: $(SUFFIXES) .

PROG = main
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

$(PROG): $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS)

.cpp.o:
	$(CC) -c $< -o $@ $(CFLAGS)

run: $(PROG)
	./$(PROG) $(ARGS)

.PHONY: clean

clean:
	$(RM) $(OBJS) $(PROG)
