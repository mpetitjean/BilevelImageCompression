CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -pedantic 

SUFFIXES = .cpp .o
.SUFFIXES: $(SUFFIXES) .

PROG = main
OBJS = $(PROG).o

all: $(PROG) 
	./$(PROG) $(ARGS)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(PROG) $(OBJS) *~ *.raw 
