
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
    } else {
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
    match(TOKEN_RBRACE);
}

void parse_print() {
    match(TOKEN_PRINT);
    match(TOKEN_IDENT);
    match(TOKEN_SEMI);
}
