SRC = $(wildcard ./src/*.c)
OBJ =  $(patsubst %.c,%.o,$(SRC))
CC  =  arm-linux-gcc
TAG = ./bin/main
CFLAGS = -L./LIB -I./inc -lfont -lm -lpthread -ljpeg 


$(TAG):$(OBJ)
	$(CC) $(^) -o $(@) $(CFLAGS) 

    #arm-linux-gcc  add.o main.o  -o main
	
clean:
	$(RM) $(OBJ)  $(TAG)