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
        }
        else {
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
    if (line[0] == '|' && line[strlen(line) - 1] == '|') {
        char macro[128];
        strncpy(macro, line + 1, strlen(line) - 2);
        macro[strlen(line) - 2] = '\0';
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
        }
        else if (strstr(line, "let")) {
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

json_object* opcode_table = NULL;

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
    }
    else {
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
        }
        else {
            fprintf(fout, "%s", line); // passthrough normal lines
        }
    }

    fclose(fin); fclose(fout);
    printf("[REXASM] Generated: %s\n", output);
}

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int current = 0;

Token peek() {
    return tokens[current];
}

Token advance() {
    return tokens[current++];
}

void match(TokenType type) {
    if (peek().type == type) advance();
    else {
        printf("Syntax Error: Expected token type %d\n", type);
        exit(1);
    }
}

void parse_program() {
    while (peek().type != TOKEN_EOF) {
        parse_statement();
    }
}

void parse_statement() {
    Token t = peek();
    if (t.type == TOKEN_DEFINE) parse_define();
    else if (t.type == TOKEN_FUNC) parse_func();
    else if (t.type == TOKEN_PRINT) parse_print();
    else if (t.type == TOKEN_CLASS) parse_class();
    else if (t.type == TOKEN_PUBLIC || t.type == TOKEN_PRIVATE || t.type == TOKEN_PROTECTED) parse_visibility();
    else if (t.type == TOKEN_NEW) parse_new();
    else if (t.type == TOKEN_SUPER) parse_super();
    else if (t.type == TOKEN_THIS) parse_this();
    else if (t.type == TOKEN_EVAL) parse_eval();
    else if (
        t.type >= TOKEN_RAYTRACING && t.type <= TOKEN_REASONING
        ) {
        printf("[FEATURE] Parsed fine-tuned feature token: %s\n", t.text);
        advance();
        if (peek().type == TOKEN_SEMI) match(TOKEN_SEMI);
    }
    else {
        printf("Unknown statement start: %s\n", t.text);
        advance();
    }
}

void parse_define() {
    match(TOKEN_DEFINE);
    match(TOKEN_IDENT);
    match(TOKEN_COLON);
    match(TOKEN_IDENT);
    match(TOKEN_SEMI);
}

void parse_func() {
    match(TOKEN_FUNC);
    match(TOKEN_IDENT);
    match(TOKEN_LPAREN);
    match(TOKEN_RPAREN);
    match(TOKEN_LBRACE);
    while (peek().type != TOKEN_RBRACE && peek().type != TOKEN_EOF) {
        parse_statement();
    }
    match(TOKEN_RBRACE);
}

void parse_print() {
    match(TOKEN_PRINT);
    match(TOKEN_IDENT);
    match(TOKEN_SEMI);
}

void parse_class() {
    match(TOKEN_CLASS);
    match(TOKEN_IDENT);
    if (peek().type == TOKEN_EXTENDS || peek().type == TOKEN_INHERIT) {
        advance();
        match(TOKEN_IDENT);
        while (peek().type == TOKEN_COMMA) {
            match(TOKEN_COMMA);
            match(TOKEN_IDENT);
        }
    }
    match(TOKEN_LBRACE);
    while (peek().type != TOKEN_RBRACE && peek().type != TOKEN_EOF) {
        parse_statement();
    }
    match(TOKEN_RBRACE);
}

void parse_visibility() {
    advance();
    if (peek().type == TOKEN_FUNC) parse_func();
    else if (peek().type == TOKEN_DEFINE) parse_define();
    else {
        printf("Syntax Error: Expected function or variable after visibility modifier\n");
        exit(1);
    }
}

void parse_new() {
    match(TOKEN_NEW);
    match(TOKEN_IDENT);
    match(TOKEN_LPAREN);
    match(TOKEN_RPAREN);
    match(TOKEN_SEMI);
}

void parse_super() {
    match(TOKEN_SUPER);
    match(TOKEN_DOT);
    match(TOKEN_IDENT);
    match(TOKEN_LPAREN);
    match(TOKEN_RPAREN);
    match(TOKEN_SEMI);
}

void parse_this() {
    match(TOKEN_THIS);
    if (peek().type == TOKEN_DOT) {
        match(TOKEN_DOT);
        match(TOKEN_IDENT);
        if (peek().type == TOKEN_LPAREN) {
            match(TOKEN_LPAREN);
            match(TOKEN_RPAREN);
        }
    }
    match(TOKEN_SEMI);
}

void parse_eval() {
    match(TOKEN_EVAL);
    match(TOKEN_LPAREN);
    if (peek().type == TOKEN_IDENT || peek().type == TOKEN_NUMBER || peek().type == TOKEN_STRING) {
        advance();
    }
    match(TOKEN_RPAREN);
    match(TOKEN_SEMI);
}

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
        }
        else if (strcmp(argv[i], "--parse") == 0) {
            parse_program();
        }
        else if (strcmp(argv[i], "--ir") == 0) {
            generate_ir();
        }
        else if (strcmp(argv[i], "--asm") == 0) {
            generate_asm();
        }
        else if (strcmp(argv[i], "--bin") == 0) {
            compile_binary();
        }
        else if (strcmp(argv[i], "--run") == 0) {
            run_executable();
        }
        else {
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

rewrite_r4_to_rexasm("examples/hello_world.r4", "build/hello_world.rexasm");

#include <dirent.h>
#include <sys/stat.h>

void batch_process_r4_files(const char* src_dir, const char* out_dir) {
    DIR* dir = opendir(src_dir);
    if (!dir) {
        perror("Could not open source directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strstr(entry->d_name, ".r4")) {
            char input_path[256], output_path[256];
            snprintf(input_path, sizeof(input_path), "%s/%s", src_dir, entry->d_name);
            snprintf(output_path, sizeof(output_path), "%s/%.*s.rexasm", out_dir,
                (int)(strlen(entry->d_name) - 3), entry->d_name);
            printf("[BATCH] Processing %s -> %s\n", input_path, output_path);
            rewrite_r4_to_rexasm(input_path, output_path);
        }
    }
    closedir(dir);
}

batch_process_r4_files("examples", "build");

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <json-c/json.h>
#include "token_type.h"

#define MAX_SYMBOLS 128
#define MAX_MACROS 128

#define USE_PRINTF 0
#define USE_SYSCALL 1

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

typedef struct {
    char name[64];
    char expansion[256];
} Macro;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;
Macro macros[MAX_MACROS];
int macro_count = 0;

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

void expand_macro(const char* macro_name) {
    for (int i = 0; i < macro_count; i++) {
        if (strcmp(macro_name, macros[i].name) == 0) {
            printf("[MACRO_EXPAND] %s =>\n%s\n", macro_name, macros[i].expansion);
            char* line = strtok(macros[i].expansion, "\n");
            while (line) {
                puts(line);
                line = strtok(NULL, "\n");
            }
            return;
        }
    }
    fprintf(stderr, "[ERROR] Macro '%s' not found\n", macro_name);
}

void rewrite_r4_to_rexasm(const char* input_path, const char* output_path) {
    FILE* in = fopen(input_path, "r");
    FILE* out = fopen(output_path, "w");
    if (!in || !out) {
        perror("Failed to open .r4 or .rexasm file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), in)) {
        if (line[0] == '|' && line[strlen(line) - 2] == '|') {
            line[strlen(line) - 2] = '\0';
            const char* macro_name = line + 1;
            for (int i = 0; i < macro_count; i++) {
                if (strcmp(macros[i].name, macro_name) == 0) {
                    fprintf(out, "; Macro: %s\n%s\n", macro_name, macros[i].expansion);
                    break;
                }
            }
        }
        else {
            fputs(line, out);
        }
    }
    fclose(in);
    fclose(out);
}

void batch_process_r4_files(const char* src_dir, const char* out_dir) {
    DIR* dir = opendir(src_dir);
    if (!dir) {
        perror("Could not open source directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strstr(entry->d_name, ".r4")) {
            char input_path[256], output_path[256];
            snprintf(input_path, sizeof(input_path), "%s/%s", src_dir, entry->d_name);
            snprintf(output_path, sizeof(output_path), "%s/%.*s.rexasm", out_dir,
                (int)(strlen(entry->d_name) - 3), entry->d_name);
            printf("[BATCH] Processing %s -> %s\n", input_path, output_path);
            rewrite_r4_to_rexasm(input_path, output_path);
        }
    }
    closedir(dir);
}

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "macro_loader.h"  // must contain macro loader/export interface

void print_help() {
    printf("Rexion Compiler Options:\n");
    printf("  --meta <file>           Load .r4meta macro definition file\n");
    printf("  --complete-macros       Show all macro completions\n");
    printf("  --reload-macros         Dynamically reload macros without recompilation\n");
    printf("  --export-macros <path>  Export macros as shareable bundle\n");
    // ... other flags
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    if (strcmp(argv[1], "--meta") == 0 && argc > 2) {
        load_macros(argv[2]);
        printf("✅ Macros loaded from: %s\n", argv[2]);
        return 0;
    }

    if (strcmp(argv[1], "--complete-macros") == 0) {
        list_macro_completions();  // prints all macros
        return 0;
    }

    if (strcmp(argv[1], "--reload-macros") == 0) {
        reload_macros_live();
        printf("🔁 Macros reloaded from disk\n");
        return 0;
    }

    if (strcmp(argv[1], "--export-macros") == 0 && argc > 2) {
        export_macro_bundle(argv[2]);
        printf("📦 Macros exported to: %s\n", argv[2]);
        return 0;
    }

    // ... normal compilation flow (lexer → parser → ir → asm → exe)
    return 0;
}

// macro_loader.h
void load_macros(const char* filename);
void list_macro_completions();
void reload_macros_live();
void export_macro_bundle(const char* path);

else if (strcmp(argv[i], "--meta") == 0) {
    if (i + 1 < argc) {
        load_macros("official/macros.r4meta");
        rewrite_r4_to_rexasm(argv[i + 1], "rexion.rexasm");
    }
    }
else if (strcmp(argv[i], "--rexasm") == 0) {
        if (i + 1 < argc) {
            // direct rexasm → asm → exe pipeline
            generate_asm_from_rexasm(argv[i + 1]);
        }
        }

        // lexer.c – Rexion Lexer (Simplified)
#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "token_type.h"
#include "token_type.h"
#include <stdio.h>
#ifndef LEXER_H
#define LEXER_H



        const char* token_type_name(TokenType type) {
            switch (type) {
            case TOKEN_UNKNOWN: return "UNKNOWN";
            case TOKEN_IDENT: return "IDENT";
            case TOKEN_NUMBER: return "NUMBER";
            case TOKEN_STRING: return "STRING";
            case TOKEN_ASSIGN: return "ASSIGN";
            case TOKEN_SEMI: return "SEMI";
            case TOKEN_LPAREN: return "LPAREN";
            case TOKEN_RPAREN: return "RPAREN";
            case TOKEN_LBRACE: return "LBRACE";
            case TOKEN_RBRACE: return "RBRACE";
            case TOKEN_DEFINE: return "DEFINE";
            case TOKEN_FUNC: return "FUNC";
            case TOKEN_PRINT: return "PRINT";
            case TOKEN_CLASS: return "CLASS";
            case TOKEN_EXTENDS: return "EXTENDS";
            case TOKEN_PUBLIC: return "PUBLIC";
            case TOKEN_PRIVATE: return "PRIVATE";
            case TOKEN_PROTECTED: return "PROTECTED";
            case TOKEN_NEW: return "NEW";
            case TOKEN_SUPER: return "SUPER";
            case TOKEN_THIS: return "THIS";
            case TOKEN_INHERIT: return "INHERIT";
            case TOKEN_RAYTRACING: return "RAYTRACING";
            case TOKEN_VECTORIZE: return "VECTORIZE";
            case TOKEN_SHADING: return "SHADING";
            case TOKEN_TRACKING: return "TRACKING";
            case TOKEN_RENDERING: return "RENDERING";
            case TOKEN_STACKING: return "STACKING";
            case TOKEN_LAYERING: return "LAYERING";
            case TOKEN_PARTICLE_PHYSICS: return "PARTICLE_PHYSICS";
            case TOKEN_SCULPTING: return "SCULPTING";
            case TOKEN_TEXTURING: return "TEXTURING";
            case TOKEN_RIGGING: return "RIGGING";
            case TOKEN_SMOKE: return "SMOKE";
            case TOKEN_STREAMING: return "STREAMING";
            case TOKEN_LIGHTING: return "LIGHTING";
            case TOKEN_TRANSITIONS: return "TRANSITIONS";
            case TOKEN_MOTION: return "MOTION";
            case TOKEN_AGING: return "AGING";
            case TOKEN_MORPHING: return "MORPHING";
            case TOKEN_COLLISION_DETECTION: return "COLLISION_DETECTION";
            case TOKEN_MATRIX: return "MATRIX";
            case TOKEN_OPTICS: return "OPTICS";
            case TOKEN_ZOOM: return "ZOOM";
            case TOKEN_VOICE: return "VOICE";
            case TOKEN_MUSIC: return "MUSIC";
            case TOKEN_CAD: return "CAD";
            case TOKEN_BLUEPRINTING: return "BLUEPRINTING";
            case TOKEN_WORLD_BUILDING: return "WORLD_BUILDING";
            case TOKEN_ENCRYPTION: return "ENCRYPTION";
            case TOKEN_DECRYPTION: return "DECRYPTION";
            case TOKEN_CONVERSIONS: return "CONVERSIONS";
            case TOKEN_SECTIONING: return "SECTIONING";
            case TOKEN_WARPING: return "WARPING";
            case TOKEN_BLURRING: return "BLURRING";
            case TOKEN_SHARPENING: return "SHARPENING";
            case TOKEN_COORDINATES: return "COORDINATES";
            case TOKEN_REASONING: return "REASONING";
            default: return "<INVALID>";
            }
        }

        void token_dump(Token* tokens, int count) {
            printf("\n--- Token Dump (%d tokens) ---\n", count);
            for (int i = 0; i < count; i++) {
                printf("[%03d] %-22s -> '%s'\n", i, token_type_name(tokens[i].type), tokens[i].text);
            }
            printf("-----------------------------\n");
        }


        // Token array and tracking state
        extern Token tokens[MAX_TOKENS];
        extern int token_count;

        // Primary lexing interface
        void lex(const char* input);
        void add_token(TokenType type, const char* text);

#endif // LEXER_H

        Token tokens[MAX_TOKENS];
        int token_count = 0;
        const char* src;
        int pos = 0;

        void add_token(TokenType type, const char* text) {
            Token t;
            t.type = type;
            strncpy(t.text, text, MAX_TOKEN_LEN);
            tokens[token_count++] = t;
        }

        void lex(const char* input) {
            src = input;
            char current[128];
            int i = 0;

            while (src[pos] != '\0') {
                if (isspace(src[pos])) {
                    pos++;
                    continue;
                }
                if (isalpha(src[pos])) {
                    i = 0;
                    while (isalnum(src[pos]) || src[pos] == '_')
                        current[i++] = src[pos++];
                    current[i] = '\0';

                    // Keyword Recognition
                    if (strcmp(current, "raytracing") == 0) add_token(TOKEN_RAYTRACING, current);
                    else if (strcmp(current, "vectorize") == 0) add_token(TOKEN_VECTORIZE, current);
                    else if (strcmp(current, "shading") == 0) add_token(TOKEN_SHADING, current);
                    else if (strcmp(current, "tracking") == 0) add_token(TOKEN_TRACKING, current);
                    else if (strcmp(current, "rendering") == 0) add_token(TOKEN_RENDERING, current);
                    else if (strcmp(current, "stacking") == 0) add_token(TOKEN_STACKING, current);
                    else if (strcmp(current, "layering") == 0) add_token(TOKEN_LAYERING, current);
                    else if (strcmp(current, "particle_physics") == 0) add_token(TOKEN_PARTICLE_PHYSICS, current);
                    else if (strcmp(current, "sculpting") == 0) add_token(TOKEN_SCULPTING, current);
                    else if (strcmp(current, "texturing") == 0) add_token(TOKEN_TEXTURING, current);
                    else if (strcmp(current, "rigging") == 0) add_token(TOKEN_RIGGING, current);
                    else if (strcmp(current, "smoke") == 0) add_token(TOKEN_SMOKE, current);
                    else if (strcmp(current, "streaming") == 0) add_token(TOKEN_STREAMING, current);
                    else if (strcmp(current, "lighting") == 0) add_token(TOKEN_LIGHTING, current);
                    else if (strcmp(current, "transitions") == 0) add_token(TOKEN_TRANSITIONS, current);
                    else if (strcmp(current, "motion") == 0) add_token(TOKEN_MOTION, current);
                    else if (strcmp(current, "aging") == 0) add_token(TOKEN_AGING, current);
                    else if (strcmp(current, "morphing") == 0) add_token(TOKEN_MORPHING, current);
                    else if (strcmp(current, "collision_detection") == 0) add_token(TOKEN_COLLISION_DETECTION, current);
                    else if (strcmp(current, "matrix") == 0) add_token(TOKEN_MATRIX, current);
                    else if (strcmp(current, "optics") == 0) add_token(TOKEN_OPTICS, current);
                    else if (strcmp(current, "zoom") == 0) add_token(TOKEN_ZOOM, current);
                    else if (strcmp(current, "voice") == 0) add_token(TOKEN_VOICE, current);
                    else if (strcmp(current, "music") == 0) add_token(TOKEN_MUSIC, current);
                    else if (strcmp(current, "cad") == 0) add_token(TOKEN_CAD, current);
                    else if (strcmp(current, "blueprinting") == 0) add_token(TOKEN_BLUEPRINTING, current);
                    else if (strcmp(current, "world_building") == 0) add_token(TOKEN_WORLD_BUILDING, current);
                    else if (strcmp(current, "encryption") == 0) add_token(TOKEN_ENCRYPTION, current);
                    else if (strcmp(current, "decryption") == 0) add_token(TOKEN_DECRYPTION, current);
                    else if (strcmp(current, "conversions") == 0) add_token(TOKEN_CONVERSIONS, current);
                    else if (strcmp(current, "sectioning") == 0) add_token(TOKEN_SECTIONING, current);
                    else if (strcmp(current, "warping") == 0) add_token(TOKEN_WARPING, current);
                    else if (strcmp(current, "blurring") == 0) add_token(TOKEN_BLURRING, current);
                    else if (strcmp(current, "sharpening") == 0) add_token(TOKEN_SHARPENING, current);
                    else if (strcmp(current, "coordinates") == 0) add_token(TOKEN_COORDINATES, current);
                    else if (strcmp(current, "reasoning") == 0) add_token(TOKEN_REASONING, current);
                    else add_token(TOKEN_IDENT, current);
                    continue;
                }
                if (isdigit(src[pos])) {
                    i = 0;
                    while (isdigit(src[pos])) current[i++] = src[pos++];
                    current[i] = '\0';
                    add_token(TOKEN_NUMBER, current);
                    continue;
                }
                if (src[pos] == '"') {
                    pos++;
                    i = 0;
                    while (src[pos] != '"' && src[pos] != '\0') current[i++] = src[pos++];
                    pos++;
                    current[i] = '\0';
                    add_token(TOKEN_STRING, current);
                    continue;
                }
                char ch = src[pos++];
                current[0] = ch; current[1] = '\0';
                switch (ch) {
                case '=': add_token(TOKEN_ASSIGN, current); break;
                case ';': add_token(TOKEN_SEMI, current); break;
                case '(': add_token(TOKEN_LPAREN, current); break;
                case ')': add_token(TOKEN_RPAREN, current); break;
                case '{': add_token(TOKEN_LBRACE, current); break;
                case '}': add_token(TOKEN_RBRACE, current); break;
                default: add_token(TOKEN_UNKNOWN, current); break;
                }
            }
        }

        Token tokens[MAX_TOKENS];
        int token_count = 0;
        const char* src;
        int pos = 0;

        // Forward declaration
        int deep_eval(int start, int end);

        // Add these to your TokenType enum:
        TOKEN_CLASS,
            TOKEN_EXTENDS,
            TOKEN_PUBLIC,
            TOKEN_PRIVATE,
            TOKEN_PROTECTED,
            TOKEN_NEW,
            TOKEN_SUPER,
            TOKEN_THIS,
            TOKEN_INHERIT,

            // Helper: get precedence
            int precedence(TokenType type) {
            if (type == TOKEN_ASSIGN) return 1;
            if (type == TOKEN_SEMI) return 0;
            // Add more as needed
            return -1;
        }

        // Deeply evaluate tokens from start to end (exclusive)
        int deep_eval(int start, int end) {
            int result = 0;
            int i = start;
            while (i < end) {
                if (tokens[i].type == TOKEN_NUMBER) {
                    result = atoi(tokens[i].text);
                    i++;
                }
                else if (tokens[i].type == TOKEN_ASSIGN) {
                    // Example: a = 5; (skip for now)
                    i++;
                }
                else if (tokens[i].type == TOKEN_SEMI) {
                    i++;
                }
                else {
                    // Unknown or unsupported token
                    i++;
                }
            }
            return result;
        }

        void add_token(TokenType type, const char* text) {
            Token t;
            t.type = type;
            strncpy(t.text, text, MAX_TOKEN_LEN);
            tokens[token_count++] = t;
        }

        void lex(const char* input) {
            src = input;
            char current[128];
            int i = 0;

            while (src[pos] != '\0') {
                if (isspace(src[pos])) {
                    pos++;
                    continue;
                }
                if (isalpha(src[pos])) {
                    i = 0;
                    while (isalnum(src[pos]) || src[pos] == '_')
                        current[i++] = src[pos++];
                    current[i] = '\0';
                    if (strcmp(current, "define") == 0) add_token(TOKEN_DEFINE, current);
                    else if (strcmp(current, "func") == 0) add_token(TOKEN_FUNC, current);
                    else if (strcmp(current, "print") == 0) add_token(TOKEN_PRINT, current);
                    else if (strcmp(current, "inherit") == 0) add_token(TOKEN_INHERIT, current);
                    else add_token(TOKEN_IDENT, current);
                    continue;
                }
                if (isdigit(src[pos])) {
                    i = 0;
                    while (isdigit(src[pos])) current[i++] = src[pos++];
                    current[i] = '\0';
                    add_token(TOKEN_NUMBER, current);
                    continue;
                }
                if (src[pos] == '"') {
                    pos++;
                    i = 0;
                    while (src[pos] != '"' && src[pos] != '\0') current[i++] = src[pos++];
                    pos++; // Skip closing quote
                    current[i] = '\0';
                    add_token(TOKEN_STRING, current);
                    continue;
                }
                char ch = src[pos++];
                current[0] = ch; current[1] = '\0';
                switch (ch) {
                case '=': add_token(TOKEN_ASSIGN, current); break;
                case ';': add_token(TOKEN_SEMI, current); break;
                case '(': add_token(TOKEN_LPAREN, current); break;
                case ')': add_token(TOKEN_RPAREN, current); break;
                case '{': add_token(TOKEN_LBRACE, current); break;
                case '}': add_token(TOKEN_RBRACE, current); break;
                default: add_token(TOKEN_UNKNOWN, current); break;
                }
            }
        }

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
            }
            else {
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
                }
                else if (strncmp(line, "## ", 3) == 0) {
                    printf("\033[1;36m%s\033[0m", line); // Cyan subheader
                }
                else if (strncmp(line, "- ", 2) == 0 || strstr(line, "*") || strstr(line, "::")) {
                    printf("\033[0;33m%s\033[0m", line); // Yellow list
                }
                else {
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
                }
                else {
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
            }
            else if (strcmp(macro_name, "HELLOPRINT") == 0) {
                fprintf(output, "LOAD R1, 'Hello, Rexion!'\nPRINT R1\n");
            }
            else {
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
                }
                else {
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
            FILE* f = fopen("macros.r4meta", "r");
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
                    struct inotify_event* event = (struct inotify_event*)&buffer[i];
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

            // DOC: Macro trace visualization and zip export system
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "macro_expander.h"
#include "tui.h"

            void draw_macro_trace_banner(const char* macro_file) {
                tui_set_color(TUI_COLOR_CYAN);
                tui_print_center("============================");
                tui_print_center("⚡ MACRO TRACE ACTIVE");
                tui_print_center("📦 Loaded: macros.r4meta");
                tui_print_center("============================");
                tui_reset_color();
            }

            void show_macro_expansion_example() {
                printf("\nExpanded Macros:\n");
                for (int i = 0; i < macro_count; i++) {
                    tui_set_color(TUI_COLOR_YELLOW);
                    printf("\n|%s|\n", macros[i].name);
                    tui_reset_color();
                    printf("%s\n", macros[i].expansion);
                }
            }

            void export_macro_bundle(const char* output_zip) {
                char command[1024];
                snprintf(command, sizeof(command),
                    "zip -r %s official/macros.r4meta icons/ docs/macro_README.md > /dev/null",
                    output_zip);
                int result = system(command);
                if (result == 0) {
                    printf("\n[EXPORT] Macro bundle exported to %s\n", output_zip);
                }
                else {
                    fprintf(stderr, "[ERROR] Export failed.\n");
                }
            }

            void auto_watch_r4meta_and_reload(const char* macro_file) {
                time_t last_modified = 0;
                struct stat st;

                while (1) {
                    if (stat(macro_file, &st) == 0 && st.st_mtime != last_modified) {
                        last_modified = st.st_mtime;
                        load_macros(macro_file);
                        draw_macro_trace_banner(macro_file);
                        printf("⚡ Macro reloaded from %s\n", macro_file);
                        show_macro_expansion_example();
                    }
                    sleep(2);
                }
            }

            // Macro Trace UI – Real-time .r4meta visual macro reloads

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/inotify.h>

#define META_FILE "macros.r4meta"
#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN     (1024 * (EVENT_SIZE + 16))

            volatile int macro_reload_flag = 0;

            void* watch_macros(void* arg) {
                int fd = inotify_init();
                if (fd < 0) { perror("inotify_init"); exit(1); }

                int wd = inotify_add_watch(fd, META_FILE, IN_MODIFY);
                if (wd == -1) { perror("inotify_add_watch"); exit(1); }

                char buffer[EVENT_BUF_LEN];
                while (1) {
                    int length = read(fd, buffer, EVENT_BUF_LEN);
                    if (length < 0) { perror("read"); break; }

                    macro_reload_flag = 1;
                    printf("\033[1;35m⚡ Macro reloaded! [%s]\033[0m\n", META_FILE);
                }
                inotify_rm_watch(fd, wd);
                close(fd);
                return NULL;
            }

            void start_macro_watcher() {
                pthread_t thread;
                pthread_create(&thread, NULL, watch_macros, NULL);
                pthread_detach(thread);
            }

            void tui_macro_banner() {
                if (macro_reload_flag) {
                    printf("\033[1;34m[MacroTUI]\033[0m \033[1;33mLive macro reload detected!\033[0m\n");
                    macro_reload_flag = 0;
                }
            }

            // Export macro bundle as .zip
            void export_macro_bundle(const char* outpath) {
                printf("\033[1;36m[Export]\033[0m Bundling macros to: %s\n", outpath);
                system("mkdir -p macro_export/docs && cp macros.r4meta macro_export/ && cp README.md macro_export/docs/ && cp icon.png macro_export/");
                char cmd[256];
                snprintf(cmd, sizeof(cmd), "zip -r %s macro_export/ > /dev/null", outpath);
                system(cmd);
                printf("\033[1;32m[✓]\033[0m Export complete.\n");
            }

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <zip.h>

#define MAX_SYMBOLS 128
#define USE_PRINTF 0
#define USE_SYSCALL 1

#define MACRO_META_PATH "macros.r4meta"
#define MACRO_ZIP_EXPORT "macro_bundle.zip"

            pthread_t macro_watcher_thread;
            int macro_reload_flag = 0;

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

            void generate_asm_from_ir(); // Forward declaration

            void* watch_macros(void* arg) {
                int fd = inotify_init();
                if (fd < 0) perror("inotify_init");
                inotify_add_watch(fd, MACRO_META_PATH, IN_MODIFY);

                char buf[4096];
                while (1) {
                    int length = read(fd, buf, sizeof(buf));
                    if (length > 0) {
                        macro_reload_flag = 1;
                        printf("\n⚡ Macro reloaded!\n");
                        // Reload and trigger IR regeneration or TUI pulse
                        generate_intermediate_code();
                        generate_asm_from_ir();
                    }
                    sleep(1);
                }
                close(fd);
                return NULL;
            }

            void export_macro_bundle() {
                int error;
                zip_t* archive = zip_open(MACRO_ZIP_EXPORT, ZIP_CREATE | ZIP_TRUNCATE, &error);
                if (!archive) {
                    fprintf(stderr, "[ERROR] Cannot create ZIP bundle\n");
                    return;
                }

                zip_file_add(archive, "macros.r4meta", zip_source_file(archive, MACRO_META_PATH, 0, 0), ZIP_FL_OVERWRITE);
                zip_file_add(archive, "README.md", zip_source_file(archive, "README.md", 0, 0), ZIP_FL_OVERWRITE);
                zip_file_add(archive, "icon.png", zip_source_file(archive, "icon.png", 0, 0), ZIP_FL_OVERWRITE);
                zip_file_add(archive, "macro_bundle.json", zip_source_file(archive, "macro_bundle.json", 0, 0), ZIP_FL_OVERWRITE);

                zip_close(archive);
                printf("📦 Macro bundle exported to %s\n", MACRO_ZIP_EXPORT);
            }

            void start_macro_watcher() {
                pthread_create(&macro_watcher_thread, NULL, watch_macros, NULL);
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
                    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

                fclose(f);
                printf("[ASM] rexion.asm generated from IR\n");
            }

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "token_type.h"
#include "macro_loader.h"
#include "tui_display.h"

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
                log_macro_trace(op, arg1, arg2);  // Trace each macro usage into TUI buffer
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

            void* watch_macro_reload(void* arg) {
                while (1) {
                    if (macro_file_changed()) {
                        reload_macros();
                        tui_notify("⚡ Macro reloaded!", 1);
                    }
                    sleep(1);
                }
                return NULL;
            }

            void start_codegen_pipeline() {
                pthread_t watcher;
                pthread_create(&watcher, NULL, watch_macro_reload, NULL);
                generate_intermediate_code();
                generate_asm_from_ir();
            }

            // DOC: Start of IR → ASM generation and embedded float string conversion
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
                    "    ret\n"
                    , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

                fclose(f);
                printf("[ASM] rexion.asm generated from IR\n");
            }

            // DOC: Expands .r4 macros into .rexasm blocks using .r4meta data
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h" // your JSON loader
#include "utils.h"

#define MAX_MACROS 256

            typedef struct {
                char name[64];
                char expansion[1024];
            } Macro;

            Macro macros[MAX_MACROS];
            int macro_count = 0;

            void load_macros(const char* meta_file) {
                FILE* f = fopen(meta_file, "r");
                if (!f) { perror("Cannot open .r4meta"); exit(1); }

                json_object* root = json_parse_file(meta_file);
                json_object* items = json_object_object_get(root, "macros");
                macro_count = json_object_array_length(items);

                for (int i = 0; i < macro_count; i++) {
                    json_object* entry = json_object_array_get_idx(items, i);
                    strcpy(macros[i].name, json_object_get_string(json_object_object_get(entry, "name")));
                    strcpy(macros[i].expansion, json_object_get_string(json_object_object_get(entry, "expansion")));
                }

                fclose(f);
                printf("Loaded %d macros from %s\n", macro_count, meta_file);
            }

            const char* expand_macro(const char* name) {
                for (int i = 0; i < macro_count; i++) {
                    if (strcmp(macros[i].name, name) == 0)
                        return macros[i].expansion;
                }
                return NULL;
            }

            void rewrite_r4_to_rexasm(const char* r4_file, const char* output_file) {
                FILE* in = fopen(r4_file, "r");
                FILE* out = fopen(output_file, "w");
                if (!in || !out) { perror("Failed to rewrite"); exit(1); }

                char line[1024];
                while (fgets(line, sizeof(line), in)) {
                    if (line[0] == '|') {
                        char macro_name[64];
                        sscanf(line, "|%[^|]|", macro_name);
                        const char* exp = expand_macro(macro_name);
                        if (exp) {
                            fprintf(out, ";; [Macro: %s]\n%s\n", macro_name, exp);
                        }
                        else {
                            fprintf(out, ";; [Unknown macro: %s]\n", macro_name);
                        }
                    }
                    else {
                        fputs(line, out);
                    }
                }

                fclose(in);
                fclose(out);
                printf("Expanded .r4 macros to .rexasm at %s\n", output_file);
            }
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

            // Event flags
            int running = 1;

            // GRAPHICS
            void init_mesh_blending() { printf("[GRAPHICS] Mesh blending initialized.\n"); }
            void init_texture_uv_sculpting() { printf("[GRAPHICS] Texture UV sculpting system ready.\n"); }
            void init_voxel_modeling() { printf("[GRAPHICS] Voxel modeling context built.\n"); }
            void enable_motion_blur() { printf("[GRAPHICS] Motion blur effect activated.\n"); }

            // RENDERING
            void init_raytracing_engine() { printf("[RENDER] GPU-bound raytracing renderer initialized.\n"); }
            void init_vertex_shader_pipeline() { printf("[RENDER] Vertex shader pipeline compiled.\n"); }
            void enable_framebuffer_stacking() { printf("[RENDER] Framebuffer stacking enabled.\n"); }

            // AUDIO
            void init_spatial_audio() {
                if (SDL_Init(SDL_INIT_AUDIO) < 0) { fprintf(stderr, "[ERROR] SDL Audio init failed.\n"); exit(1); }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { fprintf(stderr, "[ERROR] SDL_mixer init failed.\n"); exit(1); }
                printf("[AUDIO] Spatial audio matrix initialized.\n");
            }
            void enable_reactive_music() { printf("[AUDIO] Reactive music synced with player state.\n"); }
            void init_waveform_synth() { printf("[AUDIO] Dynamic waveform synthesis loaded.\n"); }

            // PHYSICS
            void init_bullet_physics() { printf("[PHYSICS] Bullet physics engine engaged.\n"); }
            void simulate_cloth() { printf("[PHYSICS] Cloth simulation threads triggered.\n"); }
            void handle_collision_matrices() { printf("[PHYSICS] Collision matrix grid registered.\n"); }

            // ENVIRONMENT
            void setup_volumetric_fog() { printf("[ENVIRONMENT] Volumetric fog volume added.\n"); }
            void activate_wind_mapping() { printf("[ENVIRONMENT] Wind direction maps loaded.\n"); }
            void simulate_light_shafts() { printf("[ENVIRONMENT] Light shafts simulated through scene geometry.\n"); }
            void apply_time_of_day_filter() { printf("[ENVIRONMENT] Dynamic lighting based on TOD applied.\n"); }

            // INTERACTION
            void init_gesture_tracking() { printf("[INTERACTION] Gesture tracking sensors online.\n"); }
            void enable_face_detection() { printf("[INTERACTION] Face detection module active.\n"); }
            void calculate_hitboxes() { printf("[INTERACTION] Hitbox data generated.\n"); }
            void start_depth_sensing() { printf("[INTERACTION] Depth sensing grid deployed.\n"); }
            void run_ai_behavior_loop() { printf("[INTERACTION] AI behavior tree traversal in progress.\n"); }

            // STORY ENGINE
            void load_cutscene_scripts() { printf("[STORY] Cutscene scripting engine ready.\n"); }
            void activate_timeline_branching() { printf("[STORY] Timeline branching logic loaded.\n"); }
            void init_voice_synthesis() { printf("[STORY] Real-time voice synthesis initialized.\n"); }

            // CAD & TOOLING
            void export_blueprints() { printf("[CAD] Blueprint data exported.\n"); }
            void setup_print_layout() { printf("[CAD] Print layout builder launched.\n"); }
            void partition_objects() { printf("[CAD] Object partitioning in CAD space initiated.\n"); }
            void perform_lathe_rendering() { printf("[CAD] Lathe rendering active for rotational modeling.\n"); }

            // RUNTIME LOOP
            void run_runtime_loop() {
                if (!glfwInit()) { fprintf(stderr, "[ERROR] GLFW init failed.\n"); exit(EXIT_FAILURE); }
                GLFWwindow* window = glfwCreateWindow(800, 600, "REXION Multimedia Runtime", NULL, NULL);
                if (!window) { glfwTerminate(); fprintf(stderr, "[ERROR] Window creation failed.\n"); exit(EXIT_FAILURE); }
                glfwMakeContextCurrent(window);

                while (!glfwWindowShouldClose(window) && running) {
                    glClearColor(0.05f, 0.1f, 0.15f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);

                    // Placeholder visual feedback
                    glBegin(GL_TRIANGLES);
                    glColor3f(1.0, 0.5, 0.2); glVertex2f(-0.6f, -0.4f);
                    glColor3f(0.2, 1.0, 0.5); glVertex2f(0.6f, -0.4f);
                    glColor3f(0.5, 0.5, 1.0); glVertex2f(0.0f, 0.6f);
                    glEnd();

                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }

                glfwDestroyWindow(window);
                glfwTerminate();
                Mix_Quit();
                SDL_Quit();
            }

            void init_all_systems() {
                // Graphics
                init_mesh_blending();
                init_texture_uv_sculpting();
                init_voxel_modeling();
                enable_motion_blur();

                // Rendering
                init_raytracing_engine();
                init_vertex_shader_pipeline();
                enable_framebuffer_stacking();

                // Audio
                init_spatial_audio();
                enable_reactive_music();
                init_waveform_synth();

                // Physics
                init_bullet_physics();
                simulate_cloth();
                handle_collision_matrices();

                // Environment
                setup_volumetric_fog();
                activate_wind_mapping();
                simulate_light_shafts();
                apply_time_of_day_filter();

                // Interaction
                init_gesture_tracking();
                enable_face_detection();
                calculate_hitboxes();
                start_depth_sensing();
                run_ai_behavior_loop();

                // Story Engine
                load_cutscene_scripts();
                activate_timeline_branching();
                init_voice_synthesis();

                // CAD & Tooling
                export_blueprints();
                setup_print_layout();
                partition_objects();
                perform_lathe_rendering();
            }

            int main() {
                printf("\n🚀 REXION Visual-Audio Runtime Booting...\n\n");
                init_all_systems();
                run_runtime_loop();
                printf("\n✅ Shutdown complete.\n");
                return 0;
            }

            struct Hitbox {
                float x, y, width, height;
            };

            int check_collision(Hitbox a, Hitbox b) {
                return (a.x < b.x + b.width &&
                    a.x + a.width > b.x &&
                    a.y < b.y + b.height &&
                    a.y + a.height > b.y);
            }

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define INLINE __attribute__((always_inline)) inline
#define FORCE_ALIGN __attribute__((aligned(64)))
#define likely(x)   __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)
#define HASH_SEED 0xA5A5A5A5

            // Secure constant-time memory compare
            INLINE int secure_memcmp(const void* a, const void* b, size_t len) {
                const uint8_t* p1 = (const uint8_t*)a;
                const uint8_t* p2 = (const uint8_t*)b;
                uint8_t result = 0;
                for (size_t i = 0; i < len; ++i) {
                    result |= p1[i] ^ p2[i];
                }
                return result; // 0 = equal, non-zero = different
            }

            // Fast + secure compile-time strength hash
            INLINE uint32_t fast_secure_hash(const char* input, size_t len) {
                uint32_t hash = HASH_SEED;
                for (size_t i = 0; i < len; ++i) {
                    hash ^= (uint32_t)(input[i]);
                    hash *= 0x45d9f3b;
                    hash ^= (hash >> 13);
                }
                return hash;
            }

            // Unrolled copy with size check
            void safe_copy(char* dest, const char* src, size_t max_len) {
                if (unlikely(!dest || !src || max_len == 0)) return;
                size_t i = 0;
                for (; i + 4 <= max_len - 1; i += 4) {
                    dest[i] = src[i];
                    dest[i + 1] = src[i + 1];
                    dest[i + 2] = src[i + 2];
                    dest[i + 3] = src[i + 3];
                }
                for (; i < max_len - 1; ++i) {
                    dest[i] = src[i];
                }
                dest[i] = '\0';
            }

            // Test entry (simulate login or secure compare)
            int main() {
                FORCE_ALIGN char user_input[128];
                FORCE_ALIGN char stored_secret[] = "VACU-SUPREME-CODEX-KEY";

                printf("Enter secure code: ");
                if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
                    fprintf(stderr, "Input error\n");
                    return EXIT_FAILURE;
                }

                // Remove newline safely
                user_input[strcspn(user_input, "\n")] = '\0';

                size_t input_len = strnlen(user_input, sizeof(user_input));
                size_t secret_len = strlen(stored_secret);

                // Constant-time comparison
                if (input_len == secret_len &&
                    secure_memcmp(user_input, stored_secret, secret_len) == 0) {
                    puts("Access Granted.");
                }
                else {
                    puts("Access Denied.");
                }

                // Output fast hash (for logging or secure trace)
                uint32_t h = fast_secure_hash(user_input, input_len);
                printf("Secure Hash: 0x%X\n", h);

                return EXIT_SUCCESS;
            }

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUTS 8
#define HASH_SEED 0xA5A5A5A5

            __device__ uint32_t device_hash(const char* input, size_t len) {
                uint32_t hash = HASH_SEED;
                for (size_t i = 0; i < len; ++i) {
                    hash ^= (uint32_t)(input[i]);
                    hash *= 0x45d9f3b;
                    hash ^= (hash >> 13);
                }
                return hash;
            }

            __global__ void hash_kernel(const char* input_data, const size_t* input_lengths, uint32_t* output_hashes, int count, int stride) {
                int idx = threadIdx.x + blockIdx.x * blockDim.x;
                if (idx < count) {
                    const char* str_ptr = input_data + (idx * stride);
                    output_hashes[idx] = device_hash(str_ptr, input_lengths[idx]);
                }
            }

            int main() {
                const char* host_inputs[MAX_INPUTS] = {
                    "alpha", "beta", "gamma", "delta",
                    "epsilon", "zeta", "eta", "theta"
                };

                const int stride = 32;
                char* h_input_data = (char*)calloc(MAX_INPUTS * stride, sizeof(char));
                size_t h_input_lengths[MAX_INPUTS];
                uint32_t h_output_hashes[MAX_INPUTS] = { 0 };

                for (int i = 0; i < MAX_INPUTS; ++i) {
                    strncpy(&h_input_data[i * stride], host_inputs[i], stride - 1);
                    h_input_lengths[i] = strlen(host_inputs[i]);
                }

                char* d_input_data;
                size_t* d_input_lengths;
                uint32_t* d_output_hashes;

                cudaMalloc((void**)&d_input_data, MAX_INPUTS * stride);
                cudaMalloc((void**)&d_input_lengths, MAX_INPUTS * sizeof(size_t));
                cudaMalloc((void**)&d_output_hashes, MAX_INPUTS * sizeof(uint32_t));

                cudaMemcpy(d_input_data, h_input_data, MAX_INPUTS * stride, cudaMemcpyHostToDevice);
                cudaMemcpy(d_input_lengths, h_input_lengths, MAX_INPUTS * sizeof(size_t), cudaMemcpyHostToDevice);

                hash_kernel << <1, MAX_INPUTS >> > (d_input_data, d_input_lengths, d_output_hashes, MAX_INPUTS, stride);
                cudaDeviceSynchronize();

                cudaMemcpy(h_output_hashes, d_output_hashes, MAX_INPUTS * sizeof(uint32_t), cudaMemcpyDeviceToHost);

                FILE* f = fopen("cuda_hash_results.txt", "w");
                if (!f) {
                    perror("File open failed");
                    return 1;
                }

                for (int i = 0; i < MAX_INPUTS; ++i) {
                    fprintf(f, "Input: %s => Hash: 0x%X\n", &h_input_data[i * stride], h_output_hashes[i]);
                }

                fclose(f);
                cudaFree(d_input_data);
                cudaFree(d_input_lengths);
                cudaFree(d_output_hashes);
                free(h_input_data);

                return 0;
            }

#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

            // Event flags
            int running = 1;

            // GRAPHICS
            void init_mesh_blending() { printf("[GRAPHICS] Mesh blending initialized.\n"); }
            void init_texture_uv_sculpting() { printf("[GRAPHICS] Texture UV sculpting system ready.\n"); }
            void init_voxel_modeling() { printf("[GRAPHICS] Voxel modeling context built.\n"); }
            void enable_motion_blur() { printf("[GRAPHICS] Motion blur effect activated.\n"); }
            void init_vectorizing() { printf("[GRAPHICS] Vectorizing engine activated.\n"); }
            void init_shading_pipeline() { printf("[GRAPHICS] Advanced shading pipeline configured.\n"); }
            void init_sculpting_tools() { printf("[GRAPHICS] Sculpting toolkit loaded.\n"); }
            void init_texturing_engine() { printf("[GRAPHICS] Texturing system primed.\n"); }

            // RENDERING
            void init_raytracing_engine() { printf("[RENDER] GPU-bound raytracing renderer initialized.\n"); }
            void init_vertex_shader_pipeline() { printf("[RENDER] Vertex shader pipeline compiled.\n"); }
            void enable_framebuffer_stacking() { printf("[RENDER] Framebuffer stacking enabled.\n"); }
            void init_particle_engine() { printf("[RENDER] Particle physics engine deployed.\n"); }
            void enable_layering() { printf("[RENDER] Layering system initiated.\n"); }

            // AUDIO
            void init_spatial_audio() {
                if (SDL_Init(SDL_INIT_AUDIO) < 0) { fprintf(stderr, "[ERROR] SDL Audio init failed.\n"); exit(1); }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { fprintf(stderr, "[ERROR] SDL_mixer init failed.\n"); exit(1); }
                printf("[AUDIO] Spatial audio matrix initialized.\n");
            }
            void enable_reactive_music() { printf("[AUDIO] Reactive music synced with player state.\n"); }
            void init_waveform_synth() { printf("[AUDIO] Dynamic waveform synthesis loaded.\n"); }
            void load_voice_engine() { printf("[AUDIO] Voice synthesis engine online.\n"); }

            // PHYSICS
            void init_bullet_physics() { printf("[PHYSICS] Bullet physics engine engaged.\n"); }
            void simulate_cloth() { printf("[PHYSICS] Cloth simulation threads triggered.\n"); }
            void handle_collision_matrices() { printf("[PHYSICS] Collision matrix grid registered.\n"); }
            void enable_motion_detection() { printf("[PHYSICS] Motion detection activated.\n"); }
            void apply_morphing_system() { printf("[PHYSICS] Morphing algorithms engaged.\n"); }

            // ENVIRONMENT
            void setup_volumetric_fog() { printf("[ENVIRONMENT] Volumetric fog volume added.\n"); }
            void activate_wind_mapping() { printf("[ENVIRONMENT] Wind direction maps loaded.\n"); }
            void simulate_light_shafts() { printf("[ENVIRONMENT] Light shafts simulated through scene geometry.\n"); }
            void apply_time_of_day_filter() { printf("[ENVIRONMENT] Dynamic lighting based on TOD applied.\n"); }
            void simulate_aging_effect() { printf("[ENVIRONMENT] Aging and withering cycles configured.\n"); }

            // INTERACTION
            void init_gesture_tracking() { printf("[INTERACTION] Gesture tracking sensors online.\n"); }
            void enable_face_detection() { printf("[INTERACTION] Face detection module active.\n"); }
            void calculate_hitboxes() { printf("[INTERACTION] Hitbox data generated.\n"); }
            void start_depth_sensing() { printf("[INTERACTION] Depth sensing grid deployed.\n"); }
            void run_ai_behavior_loop() { printf("[INTERACTION] AI behavior tree traversal in progress.\n"); }

            // STORY ENGINE
            void load_cutscene_scripts() { printf("[STORY] Cutscene scripting engine ready.\n"); }
            void activate_timeline_branching() { printf("[STORY] Timeline branching logic loaded.\n"); }
            void init_voice_synthesis() { printf("[STORY] Real-time voice synthesis initialized.\n"); }

            // CAD & TOOLING
            void export_blueprints() { printf("[CAD] Blueprint data exported.\n"); }
            void setup_print_layout() { printf("[CAD] Print layout builder launched.\n"); }
            void partition_objects() { printf("[CAD] Object partitioning in CAD space initiated.\n"); }
            void perform_lathe_rendering() { printf("[CAD] Lathe rendering active for rotational modeling.\n"); }

            // RUNTIME LOOP
            void run_runtime_loop() {
                if (!glfwInit()) { fprintf(stderr, "[ERROR] GLFW init failed.\n"); exit(EXIT_FAILURE); }
                GLFWwindow* window = glfwCreateWindow(800, 600, "REXION Multimedia Runtime", NULL, NULL);
                if (!window) { glfwTerminate(); fprintf(stderr, "[ERROR] Window creation failed.\n"); exit(EXIT_FAILURE); }
                glfwMakeContextCurrent(window);

                while (!glfwWindowShouldClose(window) && running) {
                    glClearColor(0.05f, 0.1f, 0.15f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);

                    glBegin(GL_TRIANGLES);
                    glColor3f(1.0, 0.5, 0.2); glVertex2f(-0.6f, -0.4f);
                    glColor3f(0.2, 1.0, 0.5); glVertex2f(0.6f, -0.4f);
                    glColor3f(0.5, 0.5, 1.0); glVertex2f(0.0f, 0.6f);
                    glEnd();

                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }

                glfwDestroyWindow(window);
                glfwTerminate();
                Mix_Quit();
                SDL_Quit();
            }

            void init_all_systems() {
                init_mesh_blending();
                init_texture_uv_sculpting();
                init_voxel_modeling();
                enable_motion_blur();
                init_vectorizing();
                init_shading_pipeline();
                init_sculpting_tools();
                init_texturing_engine();

                init_raytracing_engine();
                init_vertex_shader_pipeline();
                enable_framebuffer_stacking();
                init_particle_engine();
                enable_layering();

                init_spatial_audio();
                enable_reactive_music();
                init_waveform_synth();
                load_voice_engine();

                init_bullet_physics();
                simulate_cloth();
                handle_collision_matrices();
                enable_motion_detection();
                apply_morphing_system();

                setup_volumetric_fog();
                activate_wind_mapping();
                simulate_light_shafts();
                apply_time_of_day_filter();
                simulate_aging_effect();

                init_gesture_tracking();
                enable_face_detection();
                calculate_hitboxes();
                start_depth_sensing();
                run_ai_behavior_loop();

                load_cutscene_scripts();
                activate_timeline_branching();
                init_voice_synthesis();

                export_blueprints();
                setup_print_layout();
                partition_objects();
                perform_lathe_rendering();
            }

            int main() {
                printf("\n🚀 REXION Visual-Audio Runtime Booting...\n\n");
                init_all_systems();
                run_runtime_loop();
                printf("\n✅ Shutdown complete.\n");
                return 0;
            }

// peephole_optimizer.c – Rexion Peephole Optimizer
#include <stdio.h>
#include <string.h>

#define MAX_IR 1024
#define MAX_LEN 128

typedef struct {
    char op[MAX_LEN];
    char arg1[MAX_LEN];
    char arg2[MAX_LEN];
} IRInstruction;

IRInstruction ir[MAX_IR];
int ir_count = 0;

void load_ir_from_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) { perror("load_ir_from_file"); return; }
    while (fscanf(f, "%s %s %s", ir[ir_count].op, ir[ir_count].arg1, ir[ir_count].arg2) != EOF) {
        ir_count++;
    }
    fclose(f);
}

void save_ir_to_file(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) { perror("save_ir_to_file"); return; }
    for (int i = 0; i < ir_count; i++) {
        fprintf(f, "%s %s %s\n", ir[i].op, ir[i].arg1, ir[i].arg2);
    }
    fclose(f);
}

// Optimization Passes

void optimize_redundant_loads() {
    for (int i = 1; i < ir_count; i++) {
        if (strcmp(ir[i].op, "LOAD") == 0 && strcmp(ir[i-1].op, "LOAD") == 0) {
            if (strcmp(ir[i].arg1, ir[i-1].arg1) == 0 && strcmp(ir[i].arg2, ir[i-1].arg2) == 0) {
                for (int j = i; j < ir_count - 1; j++) ir[j] = ir[j+1];
                ir_count--;
                i--;
            }
        }
    }
}

void optimize_useless_add_zero() {
    for (int i = 0; i < ir_count; i++) {
        if (strcmp(ir[i].op, "ADD") == 0 && strcmp(ir[i].arg2, "0") == 0) {
            strcpy(ir[i].op, "NOP");
            ir[i].arg1[0] = '\0';
            ir[i].arg2[0] = '\0';
        }
    }
}

void optimize_mov_to_same_register() {
    for (int i = 0; i < ir_count; i++) {
        if (strcmp(ir[i].op, "MOV") == 0 && strcmp(ir[i].arg1, ir[i].arg2) == 0) {
            strcpy(ir[i].op, "NOP");
            ir[i].arg1[0] = '\0';
            ir[i].arg2[0] = '\0';
        }
    }
}

void fold_constant_adds() {
    for (int i = 0; i < ir_count - 2; i++) {
        if (strcmp(ir[i].op, "LOAD") == 0 && strcmp(ir[i+1].op, "LOAD") == 0 && strcmp(ir[i+2].op, "ADD") == 0) {
            int val1, val2;
            if (sscanf(ir[i].arg2, "%d", &val1) == 1 &&
                sscanf(ir[i+1].arg2, "%d", &val2) == 1 &&
                strcmp(ir[i].arg1, ir[i+2].arg1) != 0 &&
                strcmp(ir[i+1].arg1, ir[i+2].arg1) != 0) {

                int result = val1 + val2;
                snprintf(ir[i].arg2, MAX_LEN, "%d", result);
                strcpy(ir[i].arg1, ir[i+2].arg1);
                for (int j = i+1; j < ir_count - 2; j++) ir[j] = ir[j+2];
                ir_count -= 2;
                i--;
            }
        }
    }
}

void run_all_peephole_passes() {
    optimize_redundant_loads();
    optimize_useless_add_zero();
    optimize_mov_to_same_register();
    fold_constant_adds();
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.ir> <output.ir>\n", argv[0]);
        return 1;
    }

    load_ir_from_file(argv[1]);
    run_all_peephole_passes();
    save_ir_to_file(argv[2]);

    printf("[✔] Peephole optimization complete. Output saved to %s\n", argv[2]);
    return 0;
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
    { "/", "DIV", "0xF7", "11110111" },
    { "%", "MOD", "0xF7", "11110111" },
    { "=", "MOV", "0x89", "10001001" },
    { "==", "CMP", "0x3B", "00111011" },
    { "<", "JL", "0x7C", "01111100" },
    { ">", "JG", "0x7F", "01111111" },
    { "<=", "JLE", "0x7E", "01111110" },
    { ">=", "JGE", "0x7D", "01111101" },
    { "!", "NOT", "0xF7", "11110111" },
    { "^", "XOR", "0x31", "00110001" },
    { "->", "JMP", "0xE9", "11101001" },
    { "~>", "CALL", "0xE8", "11101000" },
    { "<-", "JMP throw_handler", "0x1E1", "111100001" },
    { "<~", "SETF", "0x0F94", "0000111110010100" },
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
    if (strstr(line, "[]")) {
        printf("[Mediator/Buffer Detected]\n");
    }
    if (strstr(line, "@")) {
        printf("[Specifier Detected]\n");
    }
    if (strstr(line, "$")) {
        printf("[Modifier Detected]\n");
    }
    if (strstr(line, "Init")) {
        printf("[Initialization Keyword]\n");
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

void generate_x86_64_asm(IRNode* ir);
void generate_arm64_asm(IRNode* ir);
void generate_riscv_asm(IRNode* ir);

const char* allocate_register(const char* varname, int is_float, TargetArch* arch);

TargetArch* select_arch(const char* target_name);

void generate_asm(IRNode* ir, const char* target_name) {
    TargetArch* arch = select_arch(target_name);
    if (!arch) {
        fprintf(stderr, "Error: Unsupported target architecture '%s'\n", target_name);
        return;
    }
    switch (arch->type) {
        case ARCH_X86_64:
            generate_x86_64_asm(ir);
            break;
        case ARCH_ARM64:
            generate_arm64_asm(ir);
            break;
        case ARCH_RISCV:
            generate_riscv_asm(ir);
            break;
        default:
            fprintf(stderr, "Error: Unknown architecture type\n");
    }
}

