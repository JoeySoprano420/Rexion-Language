#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void stage_ultra_ir() {
    printf("[IR] CLASS Animal\n");
    printf("[IR]   FIELD public name : String\n");
    printf("[IR]   METHOD public speak() { PRINT \"...\" }\n");
    printf("[IR] ENDCLASS\n");

    printf("[IR] CLASS Dog INHERIT Animal, Pet\n");
    printf("[IR]   FIELD private breed : String\n");
    printf("[IR]   METHOD public speak() { PRINT \"Woof!\" }\n");
    printf("[IR]   METHOD public fetch() { PRINT \"Fetching!\" }\n");
    printf("[IR] ENDCLASS\n");

    printf("[IR] EVAL (1 + 2 * 3)\n");
    printf("[IR] STORE <eval_result>, result\n");
    printf("[IR] PRINT result\n");

    printf("[IR] NEW Dog AS myDog\n");
    printf("[IR] CALL myDog.speak()\n");
    printf("[IR] CALL myDog.fetch()\n");
    printf("[IR] CALL super.speak()\n");
    printf("[IR] CALL this.speak()\n");
}

void print_banner() {
    printf("rexionc - REXION Language Compiler v1.0\n");
}

void run_stage(const char* label, void (*stage_fn)()) {
    printf(">>> %s...\n", label);
    stage_fn();
}

void stage_lex() {
    printf("[LEX] Tokenizing source... (stub)\n");
}

void stage_parse() {
    printf("[PARSE] Building AST... (stub)\n");
}

void stage_ir() {
    printf("[IR] Generating IR... (stub)\n");
}

void stage_codegen() {
    printf("[ASM] Generating NASM output...\n");
    FILE* f = fopen("hello.asm", "w");
    if (!f) {
        perror("Could not write ASM file");
        exit(1);
    }
    fprintf(f,
        "section .data\n"
        "message db \"Hello, Rexion!\", 0\n"
        "section .text\n"
        "global _start\n"
        "_start:\n"
        "    mov rsi, message\n"
        "    call print_string\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "print_string:\n"
        "    mov rdx, 15\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    syscall\n"
        "    ret\n"
    );
    fclose(f);
}

void stage_compile() {
    system("nasm -felf64 hello.asm -o hello.o");
    system("gcc -no-pie hello.o -o hello.exe");
    printf("✅ Executable built: hello.exe\n");
}

void stage_run() {
    printf("🚀 Running hello.exe:\n\n");
    system("./hello.exe");
}

int main(int argc, char** argv) {
    print_banner();

    if (argc < 2) {
        printf("Usage: %s [--lex] [--ast] [--ir] [--asm] [--bin] [--run] [--ultra-ir]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--lex") == 0)
            run_stage("Lexing", stage_lex);
        else if (strcmp(argv[i], "--ast") == 0)
            run_stage("Parsing", stage_parse);
        else if (strcmp(argv[i], "--ir") == 0)
            run_stage("IR Generation", stage_ir);
        else if (strcmp(argv[i], "--asm") == 0)
            run_stage("Codegen", stage_codegen);
        else if (strcmp(argv[i], "--bin") == 0)
            run_stage("Binary Compile", stage_compile);
        else if (strcmp(argv[i], "--run") == 0)
            run_stage("Execution", stage_run);
        else if (strcmp(argv[i], "--ultra-ir") == 0)
            run_stage("Ultra Inheritance & Deep OOP IR", stage_ultra_ir);
        else
            printf("Unknown option: %s\n", argv[i]);
    }

    return 0;
}
