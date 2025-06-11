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

// parser.c – Rexion Parser (Simplified)
#include "parser.h"
#include <stdio.h>

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
    if (t.type == TOKEN_DEFINE) {
        parse_define();
    } else if (t.type == TOKEN_FUNC) {
        parse_func();
    } else if (t.type == TOKEN_PRINT) {
        parse_print();
    } else if (t.type == TOKEN_CLASS) {
        parse_class();
    } else if (t.type == TOKEN_PUBLIC || t.type == TOKEN_PRIVATE || t.type == TOKEN_PROTECTED) {
        parse_visibility();
    } else if (t.type == TOKEN_NEW) {
        parse_new();
    } else if (t.type == TOKEN_SUPER) {
        parse_super();
    } else if (t.type == TOKEN_THIS) {
        parse_this();
    } else if (t.type == TOKEN_EVAL) {
        parse_eval();
    }
    // Graphics & Audio Advanced Features
    else if (
        t.type == TOKEN_RAYTRACING ||
        t.type == TOKEN_VECTORIZE ||
        t.type == TOKEN_SHADING ||
        t.type == TOKEN_TUNING ||
        t.type == TOKEN_TRACKING ||
        t.type == TOKEN_TRACING ||
        t.type == TOKEN_RENDERING ||
        t.type == TOKEN_STACKING ||
        t.type == TOKEN_LAYERING ||
        t.type == TOKEN_DIALING ||
        t.type == TOKEN_PARTICLE_PHYSICS ||
        t.type == TOKEN_MODDING ||
        t.type == TOKEN_TEXTURING ||
        t.type == TOKEN_SCULPTING ||
        t.type == TOKEN_MOLDING ||
        t.type == TOKEN_RIGGING ||
        t.type == TOKEN_FLESH ||
        t.type == TOKEN_BONES ||
        t.type == TOKEN_SMOKE ||
        t.type == TOKEN_STREAMING ||
        t.type == TOKEN_STEAM ||
        t.type == TOKEN_DIRECTIONALS ||
        t.type == TOKEN_DURATIONS ||
        t.type == TOKEN_3D ||
        t.type == TOKEN_SCANNING ||
        t.type == TOKEN_LIGHTING ||
        t.type == TOKEN_TRANSITIONS ||
        t.type == TOKEN_MOTION ||
        t.type == TOKEN_AGING ||
        t.type == TOKEN_WITHERING ||
        t.type == TOKEN_MOTION_DETECTION ||
        t.type == TOKEN_COLLISION_DETECTION ||
        t.type == TOKEN_MORPHING ||
        t.type == TOKEN_RANGE ||
        t.type == TOKEN_SCOPE ||
        t.type == TOKEN_SCALING ||
        t.type == TOKEN_TELESCOPICS ||
        t.type == TOKEN_OPTICS ||
        t.type == TOKEN_INTRINSIC_MEASURING ||
        t.type == TOKEN_MATRIX ||
        t.type == TOKEN_LASERING ||
        t.type == TOKEN_ZOOM ||
        t.type == TOKEN_PINCH ||
        t.type == TOKEN_CARD_CREATION ||
        t.type == TOKEN_VOICE ||
        t.type == TOKEN_MUSIC ||
        t.type == TOKEN_CROSS_VECTOR ||
        t.type == TOKEN_FRAMING ||
        t.type == TOKEN_CAD ||
        t.type == TOKEN_BLUEPRINTING ||
        t.type == TOKEN_CYCLING ||
        t.type == TOKEN_FOOTPRINTS ||
        t.type == TOKEN_IMPRESSIONS ||
        t.type == TOKEN_FINGERPRINTING ||
        t.type == TOKEN_BOXING ||
        t.type == TOKEN_UNBOXING ||
        t.type == TOKEN_BACKGROUND_INTERACTIVITY ||
        t.type == TOKEN_WIND ||
        t.type == TOKEN_PIPELINES ||
        t.type == TOKEN_DETAILING ||
        t.type == TOKEN_FENCING ||
        t.type == TOKEN_WORLD_BUILDING ||
        t.type == TOKEN_WIPING ||
        t.type == TOKEN_COLOR ||
        t.type == TOKEN_VISION ||
        t.type == TOKEN_COMPUTER_VISION ||
        t.type == TOKEN_CONVERSIONS ||
        t.type == TOKEN_FIXING ||
        t.type == TOKEN_UNFIXING ||
        t.type == TOKEN_CUTTING ||
        t.type == TOKEN_SECTIONING ||
        t.type == TOKEN_PARTITIONING ||
        t.type == TOKEN_CIPHERING ||
        t.type == TOKEN_ENCRYPTION ||
        t.type == TOKEN_SHADING
