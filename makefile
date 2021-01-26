CC=x86_64-w64-mingw32-gcc

all: udiv div

udiv: udiv.o
	$(CC) -o udiv udiv.o

div: div.o
	$(CC) -o div div.o

clean:
	$(RM) *.o *.exe
