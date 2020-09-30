CC=gcc 
CFLAGS=-c

all: comp

comp: main.o parser.o treePrint.o scanner.o generateCode.o
	$(CC) -o comp main.o parser.o treePrint.o scanner.o generateCode.o 

main.o: main.c
	$(CC) $(CFLAGS) main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) parser.c

treePrint.o: treePrint.c treePrint.h
	$(CC) $(CFLAGS) treePrint.c

scanner.o: scanner.c scanner.h 
	$(CC) $(CFLAGS) scanner.c

generateCode.o: generateCode.c generateCode.h
	$(CC) $(CFLAGS) generateCode.c

clean:
	rm *.o comp
