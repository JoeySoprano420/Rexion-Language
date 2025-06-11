#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "token_debug.c" // token_dump()

// Mock IR and ASM generation for demonstration
void generate_ir() {
    printf("[IR] Generating Intermediate Representation...\n");
    printf("[IR] FUNC main() { ... }\n");
    printf("[IR] RETURN 0\n");
}

void generate_asm() {
    printf("[ASM] Emitting NASM x86_64 Assembly...\n");
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("ASM output failed"); exit(1); }
    fprintf(f,
        "section .text\n"
        "global _start\n"
        "_start:\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
    );
    fclose(f);
}

void compile_binary() {
    printf("[BIN] Assembling and Linking...\n");
    system("nasm -felf64 rexion.asm -o rexion.o");
    system("gcc -no-pie rexion.o -o rexion.exe");
    printf("✅ Output: rexion.exe\n");
}

void run_executable() {
    printf("🚀 Running rexion.exe:\n\n");
    system("./rexion.exe");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <source.rex> [--tokens] [--parse] [--ir] [--asm] [--bin] [--run]\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Source file error");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* source = malloc(length + 1);
    fread(source, 1, length, file);
    source[length] = '\0';
    fclose(file);

    lex(source);

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--tokens") == 0) {
            token_dump(tokens, token_count);
        } else if (strcmp(argv[i], "--parse") == 0) {
            parse_program();
        } else if (strcmp(argv[i], "--ir") == 0) {
            generate_ir();
        } else if (strcmp(argv[i], "--asm") == 0) {
            generate_asm();
        } else if (strcmp(argv[i], "--bin") == 0) {
            compile_binary();
        } else if (strcmp(argv[i], "--run") == 0) {
            run_executable();
        } else {
            printf("Unknown option: %s\n", argv[i]);
        }
    }

    free(source);
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    const char* symbol;
    const char* asm_op;
    const char* hex;
    const char* bin;
} SymbolMap;

SymbolMap intrinsic_map[] = {
    { "and", "AND", "0x1D2", "111010010" },
    { "or", "OR", "0x1D3", "111010011" },
    { "xor", "XOR", "0x1D4", "111010100" },
    { "not", "NOT", "0x1D5", "111010101" },
    { "nullptr", "XOR reg, reg", "0x1E0", "111100000" },
    { "throw", "JMP throw_handler", "0x1E1", "111100001" },
    { "if", "CMP + conditional jump", "0x205", "1000000101" },
    { "new", "CALL malloc", "0x201", "1000000001" },
    { "delete", "CALL free", "0x202", "1000000010" },
    { "+", "ADD", "0x01", "00000001" },
    { "-", "SUB", "0x29", "00101001" },
    { "*", "MUL", "0xF7", "11110111" },
    { "/", "DIV", "0xF7", "11110111" }, // same opcode, different bits
    { NULL, NULL, NULL, NULL }
};

void explain_symbol(const char* input) {
    for (int i = 0; intrinsic_map[i].symbol; i++) {
        if (strcmp(intrinsic_map[i].symbol, input) == 0) {
            printf("Symbol: %s\nASM: %s\nHex: %s\nBin: %s\n",
                intrinsic_map[i].symbol,
                intrinsic_map[i].asm_op,
                intrinsic_map[i].hex,
                intrinsic_map[i].bin);
            return;
        }
    }
    printf("Unknown symbol: %s\n", input);
}

void intrinsic_parse_line(char* line) {
    if (strstr(line, "Let")) {
        printf("[Intrinsic Mapper] Variable declaration: %s\n", line);
    }
    if (strstr(line, "==")) {
        printf("[Immutable Declaration Detected]\n");
    }
    if (strstr(line, "Start")) {
        printf("[Command Entry Point]\n");
    }
    if (strstr(line, "Return")) {
        printf("[Command Exit Detected]\n");
    }
}

#include "rexion_intrinsic_mapper.c" // or compile separately and link

void process_intrinsic_layer(char* src_line) {
    intrinsic_parse_line(src_line);
}

