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

docs:
	@echo "Generating Codex PDF and Markdown..."
	python3 tools/generate_codex.py
	@echo "Codex generated: ./rexion_language_overview.md and ./Rexion_Language_Codex.pdf"

install: build docs
	@echo "Installing everything including documentation..."

CC = gcc
NASM = nasm
LD = ld
CFLAGS = -O2 -Wall
OBJS = lexer.o parser.o rexionc_main.o ir_codegen.o
OUTPUT = rexionc

SRC = $(wildcard *.c)
HEADERS = $(wildcard *.h)
EXAMPLES = $(wildcard examples/*.r4)

.PHONY: all run clean docs examples tui

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

lexer.o: lexer.c lexer.h token_type.h
parser.o: parser.c parser.h token_type.h
rexionc_main.o: rexionc_main.c token_type.h
ir_codegen.o: ir_codegen.c token_type.h

# Run full build + test examples
run: all examples

# Compile each .r4 to IR + ASM + .exe
examples:
	@echo "🌐 Compiling .r4 Codex Examples..."
	@for file in $(EXAMPLES); do \
		echo "🔹 Compiling $$file..."; \
		./$(OUTPUT) --input="$$file" --output="build/`basename $$file .r4`.ir"; \
		nasm -f elf64 build/`basename $$file .r4`.asm -o build/`basename $$file .r4`.o; \
		ld build/`basename $$file .r4`.o -o build/`basename $$file .r4`.exe; \
	done
	@echo "✅ Examples compiled and linked."

# Generate Language Codex (.md + PDF)
docs:
	@echo "📖 Generating Language Codex..."
	python3 tools/generate_codex.py
	@echo "✅ Docs available at: rexion_language_overview.md"

# Run the interactive TUI
tui:
	@./$(OUTPUT) --tui

# Clean artifacts
clean:
	rm -f $(OUTPUT) *.o build/*.ir build/*.asm build/*.o build/*.exe
