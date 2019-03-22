CFLAGS  = -W -Wall -ansi -I /usr/include -I /usr/local/include
CC = cc

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

OBJ = UWloveimap.o net.o table.o thread.o

all: $(OBJ)
	 $(CC) $(OBJ) -o UWloveimap -lpthread

clean:
	rm -f *.o UWloveimap
	
