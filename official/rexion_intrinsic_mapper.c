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

