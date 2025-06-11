import re
from collections import namedtuple
from typing import Iterator, List, Dict, Tuple

Token = namedtuple('Token', ['type', 'value', 'line', 'column', 'length'])

KEYWORDS = {
    'func', 'return', 'if', 'else', 'elif', 'while', 'for', 'in', 'break',
    'continue', 'let', 'const', 'true', 'false', 'null', 'int', 'float',
    'bool', 'char', 'string', 'void', 'any', 'macro', 'endmacro'
}

TOKEN_SPEC = [
    ('FLOAT',     r'\d+\.\d+'),
    ('INT',       r'\d+'),
    ('STRING',    r'"(\\.|[^"\\])*"'),
    ('CHAR',      r"'(\\.|[^'\\])'"),
    ('ID',        r'[A-Za-z_][A-Za-z0-9_]*'),
    ('EQ',        r'=='), ('NE', r'!='), ('LE', r'<='), ('GE', r'>='),
    ('AND',       r'&&'), ('OR', r'\|\|'), ('NOT', r'!'),
    ('ASSIGN',    r'='), ('LT', r'<'), ('GT', r'>'),
    ('PLUS',      r'\+'), ('MINUS', r'-'), ('MUL', r'\*'), ('DIV', r'/'), ('MOD', r'%'), ('POW', r'\*\*'),
    ('LPAREN',    r'\('), ('RPAREN', r'\)'), ('LBRACE', r'\{'), ('RBRACE', r'\}'),
    ('LBRACK',    r'\['), ('RBRACK', r'\]'), ('COMMA', r','), ('DOT', r'\.'), ('SEMI', r';'), ('COLON', r':'), ('ARROW', r'->'),
    ('LINE_COMMENT', r'//[^\n]*'), ('BLOCK_COMMENT', r'/\*.*?\*/'),
    ('MACRO_DIRECTIVE', r'#\w+'),
    ('SKIP',      r'[ \t\r\n]+'),
    ('MISMATCH',  r'.')
]

token_regex = '|'.join(f'(?P<{name}>{pattern})' for name, pattern in TOKEN_SPEC)
tok_re = re.compile(token_regex, re.DOTALL | re.MULTILINE)

class LexerError(Exception):
    def __init__(self, message, line, column, snippet):
        RED = '\033[91m'; BOLD = '\033[1m'; END = '\033[0m'
        error_msg = f"{BOLD}{RED}SyntaxError: {message} at line {line}, column {column}{END}\n    {snippet}"
        super().__init__(error_msg)

def lex(source: str, debug: bool = False) -> Iterator[Token]:
    line_num, line_start, pos = 1, 0, 0
    while pos < len(source):
        match = tok_re.match(source, pos)
        if not match:
            snippet = source[pos:pos+20].split('\n')[0]
            raise LexerError(f"Illegal character '{source[pos]}'", line_num, pos - line_start + 1, snippet)
        kind, value = match.lastgroup, match.group()
        col = match.start() - line_start + 1
        if kind == 'SKIP':
            if '\n' in value: line_num += value.count('\n'); line_start = match.end()
            pos = match.end(); continue
        if kind in ('LINE_COMMENT', 'BLOCK_COMMENT'): pos = match.end(); continue
        if kind == 'ID' and value in KEYWORDS: kind = value.upper()
        if kind == 'MISMATCH':
            snippet = source[pos:pos+20].split('\n')[0]
            raise LexerError(f"Unexpected token: {value}", line_num, col, snippet)
        token = Token(kind, value, line_num, col, len(value))
        if debug: print(f"Token: {token}")
        yield token
        pos = match.end()
    yield Token('EOF', '', line_num, 1, 0)

# MACRO EXPANSION ENGINE (w/ scope + hygiene)
class MacroEngine:
    def __init__(self):
        self.macros: Dict[str, List[Token]] = {}
        self.expansion_trace: List[str] = []
        self.scopes: List[Dict[str, str]] = []

    def define_macro(self, name: str, body: List[Token]):
        self.macros[name] = body.copy()
        self.expansion_trace.append(f"Defined macro: {name} with {len(body)} tokens")

    def expand_macro(self, name: str, context_line: int) -> List[Token]:
        if name not in self.macros:
            raise LexerError(f"Unknown macro: #{name}", context_line, 1, f"#{name}")
        self.expansion_trace.append(f"Expanded macro: {name}")
        expanded = []
        self.enter_scope()
        for t in self.macros[name]:
            if t.type == 'ID':
                unique = f"{t.value}_{len(self.scopes)}"
                self.scopes[-1][t.value] = unique
                expanded.append(Token(t.type, unique, t.line, t.column, t.length))
            else:
                expanded.append(t)
        self.exit_scope()
        return expanded

    def enter_scope(self): self.scopes.append({})
    def exit_scope(self): self.scopes.pop()

    def trace(self): return self.expansion_trace

# LINTING SYSTEM
def lint_tokens(tokens: List[Token]) -> List[str]:
    warnings = []
    for i, t in enumerate(tokens):
        if t.type == 'SEMI' and i > 0 and tokens[i-1].type == 'SEMI':
            warnings.append(f"⚠️  Redundant semicolon at line {t.line}. Suggestion: remove it.")
        if t.type == 'ASSIGN' and i+1 < len(tokens) and tokens[i+1].type == 'ASSIGN':
            warnings.append(f"⚠️  Possible mistaken '==' at line {t.line}. Suggestion: Use '==' for comparison.")
    return warnings

# MACRO SYSTEM + LOOP UNROLLING
def expand_macros_and_lint(tokens: List[Token], debug=False) -> List[Token]:
    expanded = []
    macros = MacroEngine()
    i = 0

    while i < len(tokens):
        tok = tokens[i]
        if tok.type == 'MACRO':
            name = tokens[i+1].value
            body = []
            i += 2
            while tokens[i].type != 'ENDMACRO':
                body.append(tokens[i])
                i += 1
            macros.define_macro(name, body)
        elif tok.type == 'MACRO_DIRECTIVE':
            name = tok.value[1:]
            expanded.extend(macros.expand_macro(name, tok.line))
        elif tok.type == 'FOR' and i+6 < len(tokens) and tokens[i+1].type == 'ID' and tokens[i+2].type == 'IN':
            loop_var = tokens[i+1].value
            start = int(tokens[i+3].value)
            end = int(tokens[i+5].value)
            loop_body = []
            i += 6
            while tokens[i].type != 'RBRACE':
                loop_body.append(tokens[i])
                i += 1
            for val in range(start, end + 1):
                for body_tok in loop_body:
                    rep_val = str(val) if body_tok.value == loop_var else body_tok.value
                    expanded.append(Token(body_tok.type, rep_val, body_tok.line, body_tok.column, body_tok.length))
        else:
            expanded.append(tok)
        i += 1

    if debug:
        print("🔁 Macro Expansion Trace:")
        for line in macros.trace():
            print("  ", line)

    warnings = lint_tokens(expanded)
    for warn in warnings:
        print(warn)

    return expanded

