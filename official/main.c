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
