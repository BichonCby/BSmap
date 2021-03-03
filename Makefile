TARGET = BSmap2

#compiler
CC=gcc
# debug
DEBUG=-g
#optimisation
OPT=-O0
# warnings
WARN=-Wall

PTHREAD = -lpthread
MATH=-lm
 
CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

GTKLIB = `pkg-config --cflags gtk+-3.0` -DGDK_VERSION_MIN_REQUIRED=GDK_VERSION_3_0 `pkg-config --libs gtk+-3.0` -export-dynamic

# linker
LD=gcc
LDFLAGS = $(PTHREAD) $(MATH) $(GTKLIB) -export-dynamic

OBJS= BSmap2.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

BSmap2.o: BSmap2.c
	$(CC) -c $(CCFLAGS) BSmap2.c $(GTKLIB) -o BSmap2.o

clean :
	rm *.o $(TARGET)
