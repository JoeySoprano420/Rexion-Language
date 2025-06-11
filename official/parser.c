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
