#include <stdio.h>
#include <string.h>
#include "token_type.h"

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    emit_ir_operation("LOAD", "R1", "5");
    emit_ir_operation("LOAD", "R2", "3");
    emit_ir_operation("ADD", "R3", "R1 R2");
    emit_ir_operation("STORE", "result", "R3");
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 20 dup(0)\n"
        "newline db 0xA, 0\n"
        "section .text\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    mov rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 19\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
    );
    fclose(f);

    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation("PRINT_FLOAT", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 20 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    ; Floating point addition using x87\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    sub rsp, 8\n"
        "    fstp qword [rsp]\n"
        "    lea rdi, [rel fmt]\n"
        "    mov rax, 1\n"
        "    call printf\n"
        "    add rsp, 8\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 19\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
    );
    fclose(f);

    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation("PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 20 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 20 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    lea rdi, [rel fmt]\n"
        "    movq xmm0, [fltstr]\n"
        "    mov rax, 1\n"
        "    call printf\n"
        "\n"
        "    ; OR: syscall-style float-to-string output\n"
        "    ; This version assumes float already stored\n"
        "    ; Convert float manually if needed (not trivial in asm)\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 19\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
    );
    fclose(f);

    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    ; extract int and frac parts\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n"
    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    ; extract int and frac parts\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n"
    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    ; extract int and frac parts\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n"
    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    ; extract int and frac parts\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n"
    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128
#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);

    printf("\n=== Language Codex ===\n");
    FILE* codex = fopen("rexion_language_overview.md", "r");
    if (codex) {
        char line[256];
        while (fgets(line, sizeof(line), codex)) {
            if (strncmp(line, "#", 1) == 0) printf("\033[1;36m%s\033[0m", line);
            else if (strstr(line, "-") && strstr(line, ":")) printf("\033[0;33m%s\033[0m", line);
            else printf("%s", line);
        }
        fclose(codex);
    } else {
        printf("(Codex not found. Run `make docs` to generate.)\n");
    }
    printf("\n=======================\n");
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n",
        USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token_type.h"
#include "lexer.h"
#include "parser.h"

extern void generate_intermediate_code();
extern void generate_asm_from_ir();

void print_codex_colored() {
    FILE* f = fopen("rexion_language_overview.md", "r");
    if (!f) {
        perror("[ERROR] Failed to open language codex");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "# ", 2) == 0) {
            printf("\033[1;35m%s\033[0m", line); // Magenta bold header
        } else if (strncmp(line, "## ", 3) == 0) {
            printf("\033[1;36m%s\033[0m", line); // Cyan subheader
        } else if (strncmp(line, "- ", 2) == 0 || strstr(line, "*") || strstr(line, "::")) {
            printf("\033[0;33m%s\033[0m", line); // Yellow list
        } else {
            printf("%s", line);
        }
    }
    fclose(f);
}

void show_help() {
    printf("\nUsage: rexionc [options]\n\n");
    printf("  --help             Show this help message\n");
    printf("  --debug-full       Show IR, ASM, and token trace\n");
    printf("  --codex            View full language codex (TUI)\n");
    printf("  input.r4           Compile source file\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        show_help();
        return 1;
    }

    if (strcmp(argv[1], "--codex") == 0) {
        print_codex_colored();
        return 0;
    }

    if (strcmp(argv[1], "--help") == 0) {
        show_help();
        return 0;
    }

    if (strcmp(argv[1], "--debug-full") == 0) {
        printf("[DEBUG] Token dump:\n");
        extern void token_dump();
        token_dump();
        printf("\n[DEBUG] IR Generation:\n");
        generate_intermediate_code();
        printf("\n[DEBUG] ASM Generation:\n");
        generate_asm_from_ir();
        return 0;
    }

    const char* filename = argv[1];
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Failed to open source file");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char* buffer = malloc(len + 1);
    fread(buffer, 1, len, f);
    buffer[len] = '\0';
    fclose(f);

    lex(buffer);
    parse_program();
    generate_intermediate_code();
    generate_asm_from_ir();

    printf("\n[REXION] Compilation complete. Use: nasm -felf64 rexion.asm && ld rexion.o -o rexion.exe\n");
    free(buffer);
    return 0;
}
void rewrite_r4_to_rexasm(const char* input, const char* output) {
    FILE* fin = fopen(input, "r");
    FILE* fout = fopen(output, "w");

    if (!fin || !fout) {
        perror("Failed file I/O");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), fin)) {
        char macro[64];
        if (sscanf(line, "|%[^|]|", macro) == 1) {
            expand_macro_to_ir(macro, fout);
        } else {
            fprintf(fout, "%s", line); // passthrough normal lines
        }
    }

    fclose(fin); fclose(fout);
    printf("[REXASM] Generated: %s\n", output);
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void expand_macro_to_ir(const char* macro_name, FILE* output) {
    if (strcmp(macro_name, "ADDXY") == 0) {
        fprintf(output, "LOAD R1, x\nLOAD R2, y\nADD R3, R1\nADD R3, R2\nSTORE result, R3\n");
    } else if (strcmp(macro_name, "HELLOPRINT") == 0) {
        fprintf(output, "LOAD R1, 'Hello, Rexion!'\nPRINT R1\n");
    } else {
        fprintf(output, "; Unknown macro: %s\n", macro_name);
    }
}

void rewrite_r4_to_rexasm(const char* input, const char* output) {
    FILE* fin = fopen(input, "r");
    FILE* fout = fopen(output, "w");

    if (!fin || !fout) {
        perror("Failed file I/O");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), fin)) {
        char macro[64];
        if (sscanf(line, "|%[^|]|", macro) == 1) {
            fprintf(stdout, "[MACRO TRACE] Expanding: |%s|\n", macro);
            expand_macro_to_ir(macro, fout);
        } else {
            fprintf(fout, "%s", line);
        }
    }

    fclose(fin);
    fclose(fout);
    printf("[REXASM] Generated: %s\n", output);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

// Keep generate_asm_from_ir() as-is from prior version

#include <json-c/json.h>
#define MAX_MACROS 128

typedef struct {
    char name[64];
    char expansion[256];
} Macro;

Macro macros[MAX_MACROS];
int macro_count = 0;

void load_macros_from_r4meta(const char* meta_file) {
    FILE* file = fopen(meta_file, "r");
    if (!file) {
        perror("Failed to open r4meta file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    struct json_object* root = json_tokener_parse(buffer);
    struct json_object* macro_obj;

    if (json_object_object_get_ex(root, "macros", &macro_obj)) {
        int len = json_object_array_length(macro_obj);
        for (int i = 0; i < len && i < MAX_MACROS; i++) {
            struct json_object* item = json_object_array_get_idx(macro_obj, i);
            strcpy(macros[i].name, json_object_get_string(json_object_object_get(item, "name")));
            strcpy(macros[i].expansion, json_object_get_string(json_object_object_get(item, "expansion")));
            macro_count++;
        }
    }
    free(buffer);
}

load_macros_from_r4meta("official/macros.r4meta");

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"
#include "meta_loader.h"
#include "macro_expander.h"

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    ; extract int and frac parts\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n"
    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128
#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n"
    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

// New additions for macro CLI autocomplete, dynamic reload, and bundle exporter
#include <dirent.h>

void list_macros_autocomplete() {
    printf("\n[Macro Completion]\n");
    FILE *f = fopen("macros.r4meta", "r");
    if (!f) return;
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "macro")) printf("  - %s", line);
    }
    fclose(f);
}

void reload_macros_live() {
    printf("[RELOAD] Refreshing macros from macros.r4meta\n");
    // Assume live global table is cleared elsewhere
    // Call parse_r4meta("macros.r4meta"); etc.
}

void export_macro_bundle(const char* outdir) {
    printf("[EXPORT] Packing macros to %s\n", outdir);
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "cp macros.r4meta %s/macros_exported.r4meta", outdir);
    system(cmd);
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    ; extract int and frac parts\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n"
    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"
#include <dirent.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_SYMBOLS 128
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void* watch_r4meta(void* arg) {
    int fd = inotify_init();
    if (fd < 0) perror("inotify_init");

    int wd = inotify_add_watch(fd, ".", IN_MODIFY);
    char buffer[EVENT_BUF_LEN];

    while (1) {
        int length = read(fd, buffer, EVENT_BUF_LEN);
        if (length < 0) perror("read");

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (strstr(event->name, ".r4meta")) {
                    printf("\033[0;32m[⚡ Macro Reloaded: %s]\033[0m\n", event->name);
                    // TODO: call macro reload
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }
    inotify_rm_watch(fd, wd);
    close(fd);
    return NULL;
}

void start_macro_watcher() {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, watch_r4meta, NULL);
    pthread_detach(thread_id);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "    %s\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "float_to_str:\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n",
        USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"
#include "macro_loader.h" // Auto macro reload
#include "tui.h" // For visual pulse/banner integration

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    // Reload macros if updated
    if (check_macro_file_reload()) {
        load_macros();
        tui_show_macro_reload(); // ⚡ Visual pulse/banner
    }

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    ; extract int and frac parts\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n"
    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

// DOC: Symbol representation with register and float tracking
typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    ; extract int and frac parts\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n"
    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}

// DOC: Future: Connect to macro zip bundler and auto-macro trace engine

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"

#define MAX_SYMBOLS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

// DOC: Symbol structure maps variable name to register and float status
typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

// DOC: Symbol table for variable tracking
Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

// DOC: Register allocator with float-aware register hinting
const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }

    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

// DOC: Emits the IR program header
void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

// DOC: Emits one IR instruction with optional arguments
void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

// DOC: Generate IR from parsed logic and macro traceable actions
void generate_intermediate_code() {
    emit_ir_header();

    // DOC: Register allocation and symbolic value loading
    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

// DOC: Assembler emitter from IR and macro trace rewrite layer
void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    // DOC: .data and .text sections, float/print handlers, int_to_str impl
    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    ; Optional printf output\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    ; extract int and frac parts\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n

        
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "token_type.h"
#include "watch_macros.h"
#include "tui.h"

#define MAX_SYMBOLS 128
#define USE_PRINTF 0
#define USE_SYSCALL 1

// DOC: Symbol entry stores name, register, and float flag
typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

// DOC: Allocates or returns register for a variable
const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }
    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

// DOC: Emit IR code header
void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

// DOC: Emit single IR instruction
void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

// DOC: Generate example IR sequence with macro trigger
void generate_intermediate_code() {
    emit_ir_header();

    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("MACRO_TEST", "|bundle:math/core|", NULL); // DOC: Traced macro
    emit_ir_operation("HALT", NULL, NULL);
}

// DOC: Generate .asm output from IR and macros
void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        "\n"
        "float_to_str:\n"
        "    fld qword [fltstr]\n"
        "    fld st0\n"
        "    frndint\n"
        "    fsub st1, st0\n"
        "    fxch\n"
        "    fistp qword [buffer + 32]\n"
        "    fld st0\n"
        "    fmul qword [ten]\n"
        "    frndint\n"
        "    fistp qword [buffer + 40]\n"
        "    mov rdi, [buffer + 32]\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdi, buffer\n"
        "    call strlen\n"
        "    mov byte [buffer + rax], '.'\n"
        "    mov rdi, [buffer + 40]\n"
        "    mov rsi, buffer + rax + 1\n"
        "    call int_to_str\n"
        "    ret\n",
        USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
    tui_notify_macro_reload("⚡ Macro reloaded! (via IR codegen)");
}

// DOC: Triggered by main.c to start codegen pipeline
void start_codegen_pipeline() {
    pthread_t watcher;
    pthread_create(&watcher, NULL, watch_macro_file, NULL);
    generate_intermediate_code();
    generate_asm_from_ir();
}

