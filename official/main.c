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
        } else {
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
