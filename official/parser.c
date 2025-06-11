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
    // Parse function body (optional, for deep OOP)
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

// Ultra inheritance: support both 'extends' and 'inherit', allow multiple bases
void parse_class() {
    match(TOKEN_CLASS);
    match(TOKEN_IDENT);
    if (peek().type == TOKEN_EXTENDS || peek().type == TOKEN_INHERIT) {
        advance(); // consume 'extends' or 'inherit'
        match(TOKEN_IDENT);
        // Multiple inheritance: comma-separated base classes
        while (peek().type == TOKEN_COMMA) {
            match(TOKEN_COMMA);
            match(TOKEN_IDENT);
        }
    }
    match(TOKEN_LBRACE);
    // Deep OOP: parse class body (fields, methods, visibility)
    while (peek().type != TOKEN_RBRACE && peek().type != TOKEN_EOF) {
        parse_statement();
    }
    match(TOKEN_RBRACE);
}

// Parse visibility modifiers (public/private/protected)
void parse_visibility() {
    advance(); // Consume the visibility token
    // Parse a function or variable after the modifier
    if (peek().type == TOKEN_FUNC) {
        parse_func();
    } else if (peek().type == TOKEN_DEFINE) {
        parse_define();
    } else {
        printf("Syntax Error: Expected function or variable after visibility modifier\n");
        exit(1);
    }
}

// Parse 'new' keyword usage
void parse_new() {
    match(TOKEN_NEW);
    match(TOKEN_IDENT);
    match(TOKEN_LPAREN);
    match(TOKEN_RPAREN);
    match(TOKEN_SEMI);
}

// Parse 'super' keyword usage
void parse_super() {
    match(TOKEN_SUPER);
    match(TOKEN_DOT); // e.g., super.method()
    match(TOKEN_IDENT);
    match(TOKEN_LPAREN);
    match(TOKEN_RPAREN);
    match(TOKEN_SEMI);
}

// Parse 'this' keyword usage
void parse_this() {
    match(TOKEN_THIS);
    // Optionally handle member access: this.field or this.method()
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

// Deep eval: parse 'eval' statement
void parse_eval() {
    match(TOKEN_EVAL);
    match(TOKEN_LPAREN);
    // For simplicity, just parse an identifier or number inside eval
    if (peek().type == TOKEN_IDENT || peek().type == TOKEN_NUMBER || peek().type == TOKEN_STRING) {
        advance();
    }
    match(TOKEN_RPAREN);
    match(TOKEN_SEMI);
}
