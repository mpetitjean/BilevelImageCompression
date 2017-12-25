PROGRAM_NAME = main

OBJECT_FILES = $(PROGRAM_NAME).o
CFLAGS = -Wall -Wextra -pedantic -std=c++11

$(PROGRAM_NAME) : $(OBJECT_FILES)
	g++ $(CFLAGS) -o $@ $(OBJECT_FILES) -lgmp -lgmpxx

$(PROGRAM_NAME).o : $(PROGRAM_NAME).cpp
	g++ $(CFLAGS) -c $< -lgmp -lgmpxx

clean :
	$(RM) $(PROGRAM_NAME)
	$(RM) $(OBJECT_FILES)
	$(RM) *~ *.bak

run :
	@$(MAKE) && ./$(PROGRAM_NAME) $(ARGS)
