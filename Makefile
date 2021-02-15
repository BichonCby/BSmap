CC=g++
EXEC = BSmap
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
PTHREAD = -lpthread
GTKLIB = 'pkg-config --libs gtk+-3.0'
LDFLAGS = $(PTHREAD) $(GTKLIB)

all: $(EXEC)

BSmap: $(OBJ)
	$(CC) $(PTHREAD) -o $@ $^ 
	
%.o : %.cpp
	$(CC) -c $^
	
clean :
	rm *.o
