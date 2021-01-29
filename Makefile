CC=g++
EXEC = BSmap
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
LDFLAGS = -lpthread

all: $(EXEC)

BSmap: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ 
	
%.o : %.cpp
	$(CC) -c $^
	
clean :
	rm *.o
