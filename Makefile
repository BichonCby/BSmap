CC=g++
EXEC = BSmap
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
PTHREAD = -lpthread
GTKCC = `pkg-config --cflags gtk+-3.0` -DGDK_VERSION_MIN_REQUIRED=GDK_VERSION_3_0
GTKLIB = `pkg-config --cflags gtk+-3.0` -DGDK_VERSION_MIN_REQUIRED=GDK_VERSION_3_0 `pkg-config --libs gtk+-3.0`
LDFLAGS = $(PTHREAD) $(GTKLIB)

all: $(EXEC)

BSmap: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ 
	
%.o : %.cpp
	$(CC) $(GTKCC) -c $^
	
clean :
	rm *.o
