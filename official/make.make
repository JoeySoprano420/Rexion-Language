# REXION Compiler Build System

CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC = rexionc_main.c lexer_ext.c parser.c token_debug.c
HDR = lexer.h parser.h token_type.h

OBJ = $(SRC:.c=.o)

BIN = rexionc

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN) test.rex --tokens --parse --ir --asm --bin --run

clean:
	rm -f $(BIN) *.o rexion.asm rexion.o rexion.exe

.PHONY: all run clean
