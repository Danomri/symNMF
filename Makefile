CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors

symnmf: symnmf.o symnmf.h
	$(CC) -o symnmf symnmf.o -lm 

symnmf.o: symnmf.c
	$(CC) -c symnmf.c $(CFLAGS)


clean:
	rm -f mysymnmf*.so