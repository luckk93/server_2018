CC = g++ 

CFLAGS  = -g -Wall -std=c++11
LDFLAGS = -lm -lpthread 

EXEC         = server
EXEC_DISPLAY = server_display
SRC_DISPLAY  = $(wildcard *.cpp)
SRC          = $(filter-out gui.cpp, $(wildcard *.cpp))
OBJ          = $(SRC:.cpp=.o)
OBJ_DISPLAY  = $(SRC_DISPLAY:.cpp=_display.o)
DEP          = $(OBJ:.o=.d)

all: $(EXEC) $(EXEC_DISPLAY)

.PHONY: clean all

clean:
	rm -f *.o *.d $(EXEC) $(EXEC_DISPLAY)

-include $(DEP)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -MMD -o $@ $<

%_display.o: %.cpp	
	$(CC) $(CFLAGS) -c -MMD -DDISPLAY -o $@ $<

$(EXEC): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o  $@
	
$(EXEC_DISPLAY): $(OBJ_DISPLAY)
	$(CC) $^ $(LDFLAGS) -lSDL2 -lSDL2_image -o $(EXEC_DISPLAY)
