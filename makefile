CC=x86_64-w64-mingw32-gcc

all: udiv sdiv

udiv: udiv.o
	$(CC) -o udiv udiv.o

sdiv: sdiv.o
	$(CC) -o sdiv sdiv.o

clean:
	$(RM) *.o *.exe
