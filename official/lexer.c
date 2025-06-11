// lexer.c – Rexion Lexer (Simplified)
#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
