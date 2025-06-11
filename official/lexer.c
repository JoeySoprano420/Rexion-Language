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
        } else if (tokens[i].type == TOKEN_ASSIGN) {
            // Example: a = 5; (skip for now)
            i++;
        } else if (tokens[i].type == TOKEN_SEMI) {
            i++;
        } else {
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
