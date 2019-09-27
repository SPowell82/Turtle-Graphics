CFLAGS = `sdl2-config --cflags` -O2 -Wall -Wextra -pedantic -Wfloat-equal -ansi  
LIBS =  `sdl2-config --libs` -lm
INCS = interpreter.h
CC = gcc 

all: parse interp

interp: interpreter.c formsdl.c interpreter.h  
	$(CC) interpreter.c formsdl.c test_interp.c -o interp $(CFLAGS) $(LIBS)
 	
parse: parser.c interpreter.h 
	$(CC) parser.c test_parse.c -o parse $(CFLAGS) 

extension: extension.c formsdl.c interpreter.h
	$(CC) extension.c formsdl.c -o extension $(CFLAGS) $(LIBS)

clean:
	rm -f interp parse
