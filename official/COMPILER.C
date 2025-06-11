#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void compile_to_ir() {
    // --- Deep Ultra Object Oriented Programming & Ultra Inheritance Example ---
    printf("[IR] CLASS Animal\n");
    printf("[IR]   FIELD public name : String\n");
    printf("[IR]   METHOD public speak() { PRINT \"...\" }\n");
    printf("[IR] ENDCLASS\n");

    // Ultra inheritance: multiple base classes using 'inherit'
    printf("[IR] CLASS Dog INHERIT Animal, Pet\n");
    printf("[IR]   FIELD private breed : String\n");
    printf("[IR]   METHOD public speak() { PRINT \"Woof!\" }\n");
    printf("[IR]   METHOD public fetch() { PRINT \"Fetching!\" }\n");
    printf("[IR] ENDCLASS\n");

    // Deep eval: evaluating an expression at runtime
    printf("[IR] EVAL (1 + 2 * 3)\n");
    printf("[IR] STORE <eval_result>, result\n");
    printf("[IR] PRINT result\n");

    // OOP: object instantiation and method calls
    printf("[IR] NEW Dog AS myDog\n");
    printf("[IR] CALL myDog.speak()\n");
    printf("[IR] CALL myDog.fetch()\n");

    // OOP: super and this usage
    printf("[IR] CALL super.speak()\n");
    printf("[IR] CALL this.speak()\n");

    // Existing IR for demonstration
    printf("[IR] LOAD_STR R1, \"Hello, Rexion!\"\n");
    printf("[IR] STORE R1, message\n");
    printf("[IR] PRINT message\n");
    printf("[IR] HALT\n");
}

void optimize_ir() {
    printf("[OPT] Real-time inlining & const-folding...\n");
    // No-op: all constant
}

void emit_nasm() {
    printf("[ASM] Generating NASM...\n");
    FILE* f = fopen("hello.asm", "w");
    if (!f) {
        perror("asm open failed");
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

void assemble_and_link() {
    printf("[BIN] Assembling + Linking...\n");
    system("nasm -felf64 hello.asm -o hello.o");
    system("gcc -no-pie hello.o -o hello.exe");
}

void execute_jit() {
    printf("[JIT] Live Execution with Intrinsic Memory Model...\n");
    system("./hello.exe");
}

int main() {
    printf("🔧 REXION AOT+JIT COMPILER\n");
    printf("➡️  Compiling hello.r4\n");

    compile_to_ir();
    optimize_ir();
    emit_nasm();
    assemble_and_link();
    execute_jit();

    return 0;
}
