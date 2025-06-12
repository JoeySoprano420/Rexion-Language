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

void rewrite_r4_to_rexasm(const char* r4_source_path, const char* rexasm_out_path) {
    FILE* fin = fopen(r4_source_path, "r");
    FILE* fout = fopen(rexasm_out_path, "w");
    if (!fin || !fout) { perror("File IO error"); return; }

    char line[256];
    while (fgets(line, sizeof(line), fin)) {
        if (strstr(line, "|macro|")) {
            fprintf(fout, "; Macro block start\n");
            fprintf(fout, "; Translated IR block\n");
            // Translate macro to IR (stub logic here)
        } else {
            fprintf(fout, "; %s", line);  // Comment out original line for traceability
            fprintf(fout, "NOP\n"); // Stub fallback for now
        }
    }
    fclose(fin);
    fclose(fout);
    printf("[REXASM] Rewritten IR saved to %s\n", rexasm_out_path);
}

void load_opcode_table(const char* json_path) {
    FILE* f = fopen(json_path, "r");
    if (!f) { perror("Opcode JSON load failed"); return; }
    printf("[OPCODE TABLE] Loaded %s\n", json_path);
    // Stub for JSON parsing and table map registration
    fclose(f);
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
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"
#include "json_loader.h" // For loading .r4meta opcode table

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
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "%s", is_float ? "XMM" : "R");
    char suffix[2];
    snprintf(suffix, sizeof(suffix), "%d", symbol_count + 1);
    strncat(symtab[symbol_count].reg, suffix, sizeof(symtab[symbol_count].reg) - strlen(symtab[symbol_count].reg) - 1);
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

// Expand macros like |ADDXY| into a defined IR block
void expand_macro_to_ir(const char* macro) {
    if (strcmp(macro, "ADDXY") == 0) {
        emit_ir_operation("LOAD", allocate_register("x", 0), "5");
        emit_ir_operation("LOAD", allocate_register("y", 0), "3");
        emit_ir_operation("ADD", allocate_register("x", 0), allocate_register("y", 0));
    }
    // Future macros...
}

void process_r4_line(const char* line) {
    if (line[0] == '|' && line[strlen(line)-1] == '|') {
        char macro[128];
        strncpy(macro, line+1, strlen(line)-2);
        macro[strlen(line)-2] = '\0';
        expand_macro_to_ir(macro);
    }
}

void generate_intermediate_code_from_r4(const char* filename) {
    emit_ir_header();
    FILE* file = fopen(filename, "r");
    if (!file) { perror("r4 open"); return; }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "|")) {
            process_r4_line(line);
        } else if (strstr(line, "let")) {
            char var[32], val[32];
            sscanf(line, "let %s = %s", var, val);
            emit_ir_operation("LOAD", allocate_register(var, 0), val);
        }
    }

    fclose(file);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir() {
    // Unchanged — real NASM generation from earlier step
    // [omitted here for brevity, assumed retained in final file]
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
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

// === Macro Expansion Loader ===
#include <json-c/json.h>

json_object *opcode_table = NULL;

void load_opcode_table(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) { perror("Cannot open .r4meta file"); return; }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* data = malloc(len + 1);
    fread(data, 1, len, f);
    data[len] = '\0';
    fclose(f);
    opcode_table = json_tokener_parse(data);
    free(data);
}

const char* lookup_opcode(const char* macro) {
    if (!opcode_table) return NULL;
    json_object* val;
    if (json_object_object_get_ex(opcode_table, macro, &val)) {
        return json_object_get_string(val);
    }
    return NULL;
}

// === IR Emission ===
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

void expand_macro_to_ir(const char* macro) {
    const char* ir_instr = lookup_opcode(macro);
    if (ir_instr) {
        printf("[IR-MACRO] %s => %s\n", macro, ir_instr);
        printf("[IR] %s\n", ir_instr);
    } else {
        printf("[WARN] Unrecognized macro: %s\n", macro);
    }
}

void generate_intermediate_code_from_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) { perror("Failed to open .r4 file"); return; }
    char line[256];
    emit_ir_header();
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '|' && line[strlen(line) - 2] == '|') {
            line[strlen(line) - 2] = '\0';
            expand_macro_to_ir(&line[1]);
        }
    }
    fclose(f);
}

// === IR → ASM Generator ===
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
        "    ret\n",
        USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
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
#include <ctype.h>

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
    { "and_eq", "AND [mem], reg", "0x1D6", "111010110" },
    { "or_eq", "OR [mem], reg", "0x1D7", "111010111" },
    { "xor_eq", "XOR [mem], reg", "0x1D8", "111011000" },
    { "not_eq", "CMP + JNE", "0x1D9", "111011001" },
    { "reinterpret_cast", "; reinterpret context", "0x1DA", "111011010" },
    { "dynamic_cast", "; RTTI check", "0x1DB", "111011011" },
    { "static_cast", "; compile reinterpret", "0x1DC", "111011100" },
    { "const_cast", "; remove const", "0x1DD", "111011101" },
    { "is_base_of", "; trait check", "0x1DE", "111011110" },
    { "nullptr", "XOR reg, reg", "0x1E0", "111100000" },
    { "throw", "JMP throw_handler", "0x1E1", "111100001" },
    { "try", "; try label", "0x1E2", "111100010" },
    { "catch", "; catch label", "0x1E3", "111100011" },
    { "if", "CMP + conditional jump", "0x205", "1000000101" },
    { "for", "LOOP label", "0x20B", "1000001011" },
    { "new", "CALL malloc", "0x201", "1000000001" },
    { "delete", "CALL free", "0x202", "1000000010" },
    { "+", "ADD", "0x01", "00000001" },
    { "-", "SUB", "0x29", "00101001" },
    { "*", "MUL", "0xF7", "11110111" },
    { "/", "DIV", "0xF7", "11110111" }, // same opcode family
    { "%", "MOD", "0xF7", "11110111" },
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
    if (strstr(line, "#")) {
        printf("[Comment] Line skipped\n");
        return;
    }
    if (strstr(line, "**")) {
        printf("[Multiline Comment Block]\n");
        return;
    }
    if (strstr(line, "Let")) {
        printf("[Variable Declaration Detected] %s\n", line);
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
    if (strstr(line, "|")) {
        printf("[C.I.A.M.S. Macro or Definition] %s\n", line);
    }
    if (strstr(line, ";")) {
        printf("[Line Termination Detected]\n");
    }
    if (strstr(line, ":")) {
        printf("[Fallback Instruction Separator]\n");
    }
    if (strstr(line, "->")) {
        printf("[Progression Symbol Detected]\n");
    }
    if (strstr(line, "~>")) {
        printf("[Raise Expression Detected]\n");
    }
    if (strstr(line, "<~")) {
        printf("[Flag Expression Detected]\n");
    }
    if (strstr(line, "<-")) {
        printf("[Throw Command Detected]\n");
    }
}

int main(int argc, char** argv) {
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            explain_symbol(argv[i]);
        }
    } else {
        char line[256];
        printf("Enter code lines for intrinsic analysis (Ctrl+D to end):\n");
        while (fgets(line, sizeof(line), stdin)) {
            intrinsic_parse_line(line);
        }
    }
    return 0;
}
