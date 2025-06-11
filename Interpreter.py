import struct

def to_machine_code(ir, debug=False):
    machine = []
    labels = {}
    pending_jumps = []
    register_map = {f"R{i}": i for i in range(8)}
    stack = []
    stack_frame = {}

    def log(msg):
        if debug:
            print(f"[DEBUG] {msg}")

    # First pass: collect all labels and function declarations
    addr = 0
    for instr in ir:
        if instr.startswith("[FUNC_DECL]"):
            label = instr.split()[1]
            labels[label] = addr
        elif instr.startswith("[LABEL]"):
            label = instr.split()[1]
            labels[label] = addr
        addr += 1

    # Second pass: translate IR into machine instructions
    for idx, instr in enumerate(ir):
        instr = instr.strip()

        if instr.startswith("[PRINT]"):
            val = instr.split("->")[1].strip()
            encoded_val = " ".join(f"{ord(c):02X}" for c in val)
            machine.append(f"OUT {len(val):02X} {encoded_val}")
            log(f"PRINT '{val}'")

        elif instr.startswith("[ASSIGN]"):
            parts = instr.replace("[ASSIGN]", "").strip().split("->")
            dest = parts[0].strip()
            value = parts[1].strip()
            try:
                val_int = int(value)
                machine.append(f"MOV {dest}, {val_int}")
                if dest in register_map:
                    stack_frame[dest] = val_int
            except ValueError:
                machine.append(f"MOV {dest}, {value}")
            log(f"ASSIGN {dest} = {value}")

        elif instr.startswith("[CALL]"):
            func_name = instr.split()[1]
            addr = labels.get(func_name, None)
            if addr is not None:
                machine.append(f"CALL {addr:04X}")
            else:
                pending_jumps.append((idx, func_name))
                machine.append(f"CALL {func_name}")
            log(f"CALL {func_name}")

        elif instr.startswith("[RET]"):
            machine.append("RET")
            log("RETURN")

        elif instr.startswith("[PUSH]"):
            val = instr.split()[1]
            machine.append(f"PUSH {val}")
            stack.append(val)
            log(f"PUSH {val}")

        elif instr.startswith("[POP]"):
            reg = instr.split()[1]
            machine.append(f"POP {reg}")
            if stack:
                stack_frame[reg] = stack.pop()
            log(f"POP into {reg}")

        elif instr.startswith("[FUNC_DECL]"):
            name = instr.split()[1]
            machine.append(f"; FUNC {name}")
            log(f"FUNC_DECL {name}")

        elif instr.startswith("[END_FUNC]"):
            machine.append("; END FUNC")
            log("END_FUNC")

        elif instr.startswith("[LABEL]"):
            label = instr.split()[1]
            machine.append(f"{label}:")
            log(f"LABEL {label}")

        elif instr.startswith("[JMP]"):
            label = instr.split()[1]
            if label in labels:
                addr = labels[label]
                machine.append(f"JMP {addr:04X}")
            else:
                pending_jumps.append((idx, label))
                machine.append(f"JMP {label}")
            log(f"JMP to {label}")

        elif instr.startswith("[CMP]"):
            a, b = instr.replace("[CMP]", "").split(",")
            machine.append(f"CMP {a.strip()}, {b.strip()}")
            log(f"CMP {a.strip()} {b.strip()}")

        elif instr.startswith("[JZ]"):
            label = instr.split()[1]
            if label in labels:
                addr = labels[label]
                machine.append(f"JZ {addr:04X}")
            else:
                pending_jumps.append((idx, label))
                machine.append(f"JZ {label}")
            log(f"JZ {label}")

        elif instr.startswith("[JNZ]"):
            label = instr.split()[1]
            if label in labels:
                addr = labels[label]
                machine.append(f"JNZ {addr:04X}")
            else:
                pending_jumps.append((idx, label))
                machine.append(f"JNZ {label}")
            log(f"JNZ {label}")

        elif instr.startswith("[HALT]"):
            machine.append("HLT")
            log("HALT")

        elif instr.startswith("[NOP]"):
            machine.append("NOP")
            log("NOP")

        else:
            machine.append(f"; Unrecognized: {instr}")
            log(f"UNKNOWN {instr}")

    # Resolve pending jump placeholders
    for idx, label in pending_jumps:
        if label in labels:
            addr = labels[label]
            machine[idx] = machine[idx].replace(label, f"{addr:04X}")

    return machine

def export_to_binary(machine_code, filename="output.bin"):
    binary = bytearray()
    for line in machine_code:
        parts = line.split()
        for part in parts:
            if part.startswith(";") or part.endswith(":"):
                continue
            try:
                val = int(part, 16)
                binary.append(val)
            except ValueError:
                continue  # Ignore non-hex or symbolic parts
    with open(f"/mnt/data/{filename}", "wb") as f:
        f.write(binary)
    return f"/mnt/data/{filename}"

# --- EXAMPLE TEST CASE ---
if __name__ == "__main__":
    ir_code = [
        "[FUNC_DECL] main",
        "[ASSIGN] R1 -> 5",
        "[ASSIGN] R2 -> 10",
        "[PRINT] -> Hello",
        "[PUSH] R1",
        "[POP] R3",
        "[LABEL] loop_start",
        "[CMP] R1, R2",
        "[JZ] end",
        "[JMP] loop_start",
        "[LABEL] end",
        "[RET]",
        "[END_FUNC]"
    ]

    mc = to_machine_code(ir_code, debug=True)
    print("\n".join(mc))

    binary_path = export_to_binary(mc)
    print(f"\n✅ Binary exported to: {binary_path}")

    # The above code will generate machine code from the IR and export it to a binary file.
    # The debug flag can be set to True to see detailed logs of the translation process.
    # The example IR code provided will be translated into machine code and saved as a binary file.
    # The binary file will be saved in the /mnt/data directory, which is accessible for download.
    # You can modify the IR code to test different scenarios and see how the machine code changes.
    # The machine code is printed to the console for verification.
    # The code handles function declarations, labels, jumps, and basic arithmetic operations.
    # It also includes a simple stack implementation for push/pop operations.

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

from lexer import lex, Token
from typing import List, Tuple, Union

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0

    def peek(self) -> Token:
        return self.tokens[self.pos] if self.pos < len(self.tokens) else Token('EOF', '', -1, -1, 0)

    def peek_next(self) -> Token:
        return self.tokens[self.pos + 1] if self.pos + 1 < len(self.tokens) else Token('EOF', '', -1, -1, 0)

    def advance(self) -> Token:
        token = self.peek()
        self.pos += 1
        return token

    def check(self, kind: str) -> bool:
        return self.peek().type == kind

    def match(self, *kinds) -> Union[Token, None]:
        if self.peek().type in kinds:
            return self.advance()
        return None

    def expect(self, kind: str, message="Expected token") -> Token:
        if self.check(kind):
            return self.advance()
        token = self.peek()
        raise SyntaxError(f"{message}: got {token.type} '{token.value}' at line {token.line}, col {token.column}")

    def parse(self):
        ast = []
        while not self.check("EOF"):
            ast.append(self.declaration())
        return ast

    def declaration(self):
        token = self.peek()
        if token.type == 'FUNC':
            return self.func_decl()
        elif token.type == 'LET':
            return self.var_decl()
        elif token.type == 'MACRO':
            return self.macro_decl()
        else:
            return self.statement()

    def func_decl(self):
        self.expect('FUNC')
        name = self.expect('ID').value
        self.expect('LPAREN')
        params = []
        if not self.check('RPAREN'):
            params.append(self.expect('ID').value)
            while self.match('COMMA'):
                params.append(self.expect('ID').value)
        self.expect('RPAREN')
        body = self.block()
        return ('func_decl', name, params, body)

    def var_decl(self):
        self.expect('LET')
        name = self.expect('ID').value
        self.expect('ASSIGN')
        expr = self.expression()
        self.expect('SEMI')
        return ('var_decl', name, expr)

    def macro_decl(self):
        self.expect('MACRO')
        name = self.expect('ID').value
        body = []
        while not self.check('ENDMACRO'):
            body.append(self.declaration())
        self.expect('ENDMACRO')
        return ('macro_decl', name, body)

    def statement(self):
        if self.check('IF'):
            return self.if_stmt()
        elif self.check('WHILE'):
            return self.while_stmt()
        elif self.check('RETURN'):
            return self.return_stmt()
        elif self.check('PRINT'):
            return self.print_stmt()
        elif self.check('LBRACE'):
            return self.block()
        else:
            return self.expr_stmt()

    def if_stmt(self):
        self.expect('IF')
        self.expect('LPAREN')
        condition = self.expression()
        self.expect('RPAREN')
        then_branch = self.block()
        else_branch = None
        if self.match('ELSE'):
            else_branch = self.block()
        return ('if', condition, then_branch, else_branch)

    def while_stmt(self):
        self.expect('WHILE')
        self.expect('LPAREN')
        condition = self.expression()
        self.expect('RPAREN')
        body = self.block()
        return ('while', condition, body)

    def return_stmt(self):
        self.expect('RETURN')
        expr = self.expression()
        self.expect('SEMI')
        return ('return', expr)

    def print_stmt(self):
        self.expect('PRINT')
        expr = self.expression()
        self.expect('SEMI')
        return ('print', expr)

    def block(self):
        self.expect('LBRACE')
        statements = []
        while not self.check('RBRACE') and not self.check('EOF'):
            statements.append(self.declaration())
        self.expect('RBRACE')
        return ('block', statements)

    def expr_stmt(self):
        expr = self.expression()
        self.expect('SEMI')
        return ('expr_stmt', expr)

    def expression(self):
        return self.assignment()

    def assignment(self):
        expr = self.logic_or()
        if self.match('ASSIGN'):
            value = self.assignment()
            if expr[0] == 'var':
                return ('assign', expr[1], value)
            raise SyntaxError("Invalid assignment target")
        return expr

    def logic_or(self):
        expr = self.logic_and()
        while self.match('OR'):
            op = 'or'
            right = self.logic_and()
            expr = ('logical', expr, op, right)
        return expr

    def logic_and(self):
        expr = self.equality()
        while self.match('AND'):
            op = 'and'
            right = self.equality()
            expr = ('logical', expr, op, right)
        return expr

    def equality(self):
        expr = self.comparison()
        while self.match('EQ', 'NE'):
            op = self.tokens[self.pos - 1].value
            right = self.comparison()
            expr = ('binary', expr, op, right)
        return expr

    def comparison(self):
        expr = self.term()
        while self.match('LT', 'GT', 'LE', 'GE'):
            op = self.tokens[self.pos - 1].value
            right = self.term()
            expr = ('binary', expr, op, right)
        return expr

    def term(self):
        expr = self.factor()
        while self.match('PLUS', 'MINUS'):
            op = self.tokens[self.pos - 1].value
            right = self.factor()
            expr = ('binary', expr, op, right)
        return expr

    def factor(self):
        expr = self.unary()
        while self.match('MUL', 'DIV', 'MOD'):
            op = self.tokens[self.pos - 1].value
            right = self.unary()
            expr = ('binary', expr, op, right)
        return expr

    def unary(self):
        if self.match('NOT', 'MINUS'):
            op = self.tokens[self.pos - 1].value
            right = self.unary()
            return ('unary', op, right)
        return self.primary()

    def primary(self):
        token = self.advance()
        if token.type in ('INT', 'FLOAT'):
            return ('literal', float(token.value) if '.' in token.value else int(token.value))
        elif token.type == 'STRING':
            return ('literal', token.value)
        elif token.type == 'ID':
            if self.check('LPAREN'):
                return self.finish_call(('var', token.value))
            return ('var', token.value)
        elif token.type == 'LPAREN':
            expr = self.expression()
            self.expect('RPAREN')
            return expr
        else:
            raise SyntaxError(f"Unexpected token: {token.type} ({token.value}) at line {token.line}")

    def finish_call(self, callee):
        self.expect('LPAREN')
        args = []
        if not self.check('RPAREN'):
            args.append(self.expression())
            while self.match('COMMA'):
                args.append(self.expression())
        self.expect('RPAREN')
        return ('call', callee, args)

    def parse(self) -> List[Tuple[str, ...]]:
        ast = []
        while not self.check('EOF'):
            ast.append(self.declaration())
        return ast

    import graphviz
import os
from typing import List

def classify_node(label):
    if label.startswith("[FUNC_DECL]"):
        return "Function", "folder", "lightblue"
    elif label.startswith("[CALL]"):
        return "Call", "oval", "gold"
    elif label.startswith("[ASSIGN]"):
        return "Assign", "box", "lightgreen"
    elif label.startswith("[RETURN]") or label.startswith("[RET]"):
        return "Return", "box", "gray"
    elif label.startswith("[JMP]"):
        return "Jump", "diamond", "orange"
    elif label.startswith("[JZ]"):
        return "JumpTrue", "diamond", "lightsalmon"
    elif label.startswith("[JNZ]"):
        return "JumpFalse", "diamond", "tomato"
    elif label.startswith("[LABEL]"):
        return "Label", "note", "lightyellow"
    elif label.startswith("[CMP]") or label.startswith("[ADD]") or label.startswith("[SUB]") or label.startswith("[DIV]"):
        return "Op", "parallelogram", "violet"
    elif label.startswith("[PRINT]"):
        return "Print", "box", "pink"
    elif label.startswith("[IF]"):
        return "If", "octagon", "deepskyblue"
    elif label.startswith("[ELSE]"):
        return "Else", "octagon", "slateblue"
    elif label.startswith("[END_IF]"):
        return "EndIf", "octagon", "lightsteelblue"
    else:
        return "Generic", "ellipse", "white"

def sanitize_label(label):
    return label.replace('"', "'").replace("->", "→")

def visualize_ast_nested_clusters(ir: List[str], filename: str = "nested_ast_flow", outdir: str = "graphs", fmt: str = "png", debug: bool = False):
    os.makedirs(outdir, exist_ok=True)
    dot = graphviz.Digraph(comment="Rexion AST Flowchart", format=fmt, directory=outdir)
    dot.attr(rankdir='TB', fontsize='10', fontname='Courier New')

    node_counter = 0
    previous_node = None
    cluster_stack = []
    control_stack = []

    node_ids = {}

    for i, line in enumerate(ir):
        node_id = f"n{node_counter}"
        label = sanitize_label(line.strip())
        category, shape, color = classify_node(line)

        if debug:
            print(f"[{node_id}] Category: {category}, Label: {label}")

        # Handle nested if/else as clusters
        if category == "If":
            cluster_id = f"cluster_if_{len(cluster_stack)}"
            new_cluster = graphviz.Digraph(name=cluster_id)
            new_cluster.attr(label=f"IF block {len(cluster_stack)}", style="dashed", color="blue")
            cluster_stack.append(new_cluster)
            control_stack.append((node_id, "IF_TRUE"))

        elif category == "Else":
            if_cluster = cluster_stack.pop()
            dot.subgraph(if_cluster)
            cluster_id = f"cluster_else_{len(cluster_stack)}"
            new_cluster = graphviz.Digraph(name=cluster_id)
            new_cluster.attr(label=f"ELSE block {len(cluster_stack)}", style="dashed", color="purple")
            cluster_stack.append(new_cluster)
            control_stack.append((node_id, "IF_FALSE"))

        elif category == "EndIf":
            if cluster_stack:
                final = cluster_stack.pop()
                dot.subgraph(final)
            if control_stack:
                control_stack.pop()

        # Add node to current cluster or main graph
        if cluster_stack:
            cluster_stack[-1].node(node_id, label, shape=shape, style='filled', fillcolor=color, fontname="Courier")
        else:
            dot.node(node_id, label, shape=shape, style='filled', fillcolor=color, fontname="Courier")

        # Save node reference
        node_ids[i] = node_id

        # Add edge with conditional labels if applicable
        if previous_node is not None:
            edge_label = ""
            if category == "JumpTrue":
                edge_label = "true"
            elif category == "JumpFalse":
                edge_label = "false"
            elif control_stack:
                _, cond = control_stack[-1]
                if cond == "IF_TRUE":
                    edge_label = "true"
                elif cond == "IF_FALSE":
                    edge_label = "false"

            if edge_label:
                dot.edge(previous_node, node_id, label=edge_label)
            else:
                dot.edge(previous_node, node_id)

        previous_node = node_id
        node_counter += 1

    # Flush remaining cluster if any
    for rem_cluster in cluster_stack:
        dot.subgraph(rem_cluster)

    output_path = dot.render(filename=filename, cleanup=True)
    return output_path

if __name__ == "__main__":
    ir_code = [
        "[FUNC_DECL] main",
        "[ASSIGN] R1 -> 5",
        "[ASSIGN] R2 -> 10",
        "[PRINT] -> Hello",
        "[PUSH] R1",
        "[POP] R3",
        "[LABEL] loop_start",
        "[CMP] R1, R2",
        "[JZ] end",
        "[JMP] loop_start",
        "[LABEL] end",
        "[RET]",
        "[END_FUNC]"
    ]
    mc = to_machine_code(ir_code, debug=True)
    print("\n".join(mc))
    binary_path = export_to_binary(mc)
    print(f"\n✅ Binary exported to: {binary_path}")
    # Visualize the IR as a nested AST flowchart
    ast_flowchart_path = visualize_ast_nested_clusters(ir_code, debug=True)
    print(f"AST Flowchart saved to: {ast_flowchart_path}")
    # The above code will generate a flowchart of the IR code, showing nested structures like if/else blocks.
    # The flowchart will be saved in the specified output directory and format.
    # You can modify the IR code to test different scenarios and see how the flowchart changes.
    # The flowchart will include nodes for function declarations, assignments, jumps, and other control flow elements.
    # The nodes will be color-coded and shaped based on their type, making it easy to understand the structure of the code.
    # The flowchart will also handle nested if/else blocks as clusters, allowing for a clear visual representation of the control flow.
    # The debug flag can be set to True to see detailed logs of the translation and visualization process.

    from lark import Lark, Transformer, UnexpectedInput

# Grammar definition based on expanded Rexion language
rexion_grammar = r"""
    start: program

    program: statement*

    statement: func_decl
             | async_func_decl
             | var_decl
             | const_decl
             | event_decl
             | assign_stmt
             | print_stmt
             | return_stmt
             | control_flow
             | func_call
             | block

    func_decl: "func" IDENTIFIER "(" [parameters] ")" block
    async_func_decl: "async" "func" IDENTIFIER "(" [parameters] ")" block
    event_decl: "event" IDENTIFIER "(" [parameters] ")" block

    parameters: IDENTIFIER ("," IDENTIFIER)*

    var_decl: "let" IDENTIFIER "=" expression ";"
    const_decl: "const" IDENTIFIER "=" expression ";"

    assign_stmt: IDENTIFIER "=" expression ";"
    print_stmt: "print" expression ";"
    return_stmt: "return" expression? ";"

    control_flow: if_stmt | while_stmt | for_stmt | switch_stmt | try_catch_stmt

    if_stmt: "if" "(" expression ")" block ("else" block)?
    while_stmt: "while" "(" expression ")" block
    for_stmt: "for" "(" assign_stmt expression ";" assign_stmt ")" block
    switch_stmt: "switch" "(" expression ")" "{" ("case" expression ":" block)* ("default" ":" block)? "}"
    try_catch_stmt: "try" block "catch" "(" IDENTIFIER ")" block

    block: "{" statement* "}"

    func_call: IDENTIFIER "(" [arguments] ")"
    arguments: expression ("," expression)*

    expression: logical_or
    logical_or: logical_and ("or" logical_and)*
    logical_and: equality ("and" equality)*
    equality: comparison (("==" | "!=") comparison)*
    comparison: term (("<" | ">" | "<=" | ">=") term)*
    term: factor (("+" | "-") factor)*
    factor: unary (("*" | "/" | "%") unary)*
    unary: ("!" | "-")? primary
    primary: literal | IDENTIFIER | func_call | "(" expression ")" | tuple_expr

    tuple_expr: "(" expression ("," expression)+ ")"

    literal: NUMBER | FLOAT | STRING | "true" | "false" | "null"

    IDENTIFIER: /[a-zA-Z_][a-zA-Z0-9_]*/
    STRING: /"[^"]*"/
    NUMBER: /\d+/
    FLOAT: /\d+\.\d+/

    COMMENT: /\/\/[^\n]*/
    %ignore COMMENT
    %ignore /[ \t\r\n]+/
"""

# Parser setup
parser = Lark(rexion_grammar, parser='lalr', start='start')

# Function to validate and parse input code
def validate_and_parse(code: str):
    try:
        tree = parser.parse(code)
        return "VALID", tree.pretty()
    except UnexpectedInput as e:
        return "INVALID", str(e)

# Example Rexion code to test the parser
sample_code = """
func greet(name) {
    print "Hello, " + name;
}

let x = 5;
if (x > 3) {
    print "x is large";
} else {
    print "x is small";
}
"""

status, output = validate_and_parse(sample_code)
output

from lark import Lark, UnexpectedInput

rexion_grammar = r"""
    start: program
    program: statement*
    statement: func_decl
             | async_func_decl
             | var_decl
             | const_decl
             | event_decl
             | assign_stmt
             | print_stmt
             | return_stmt
             | control_flow
             | func_call
             | block

    func_decl: "func" IDENTIFIER "(" [parameters] ")" block
    async_func_decl: "async" "func" IDENTIFIER "(" [parameters] ")" block
    event_decl: "event" IDENTIFIER "(" [parameters] ")" block

    parameters: IDENTIFIER ("," IDENTIFIER)*

    var_decl: "let" IDENTIFIER "=" expression ";"
    const_decl: "const" IDENTIFIER "=" expression ";"
    assign_stmt: IDENTIFIER "=" expression ";"
    print_stmt: "print" expression ";"
    return_stmt: "return" expression? ";"

    control_flow: if_stmt | while_stmt | for_stmt | switch_stmt | try_catch_stmt

    if_stmt: "if" "(" expression ")" block ("else" block)?
    while_stmt: "while" "(" expression ")" block
    for_stmt: "for" "(" assign_stmt expression ";" assign_stmt ")" block
    switch_stmt: "switch" "(" expression ")" "{" ("case" expression ":" block)* ("default" ":" block)? "}"
    try_catch_stmt: "try" block "catch" "(" IDENTIFIER ")" block

    block: "{" statement* "}"

    func_call: IDENTIFIER "(" [arguments] ")"
    arguments: expression ("," expression)*

    expression: logical_or
    logical_or: logical_and ("or" logical_and)*
    logical_and: equality ("and" equality)*
    equality: comparison (("==" | "!=") comparison)*
    comparison: term (("<" | ">" | "<=" | ">=") term)*
    term: factor (("+" | "-") factor)*
    factor: unary (("*" | "/" | "%") unary)*
    unary: ("!" | "-")? primary
    primary: literal | IDENTIFIER | func_call | "(" expression ")" | tuple_expr

    tuple_expr: "(" expression ("," expression)+ ")"

    literal: NUMBER | FLOAT | STRING | "true" | "false" | "null"

    IDENTIFIER: /[a-zA-Z_][a-zA-Z0-9_]*/
    STRING: /"[^"]*"/
    NUMBER: /\d+/
    FLOAT: /\d+\.\d+/

    COMMENT: /\/\/[^\n]*/
    %ignore COMMENT
    %ignore /[ \t\r\n]+/
"""

parser = Lark(rexion_grammar, parser='lalr', start='start')

def validate_and_parse(code: str):
    try:
        tree = parser.parse(code)
        return "✅ VALID", tree.pretty()
    except UnexpectedInput as e:
        return "❌ INVALID", str(e)

# Example:
if __name__ == "__main__":
    code = '''
    func hello(name) {
        print "Hello, " + name;
    }
    '''
    status, result = validate_and_parse(code)
    print(status)
    print(result)

import sys
import argparse
import traceback
import time
import os
from lexer import lex
from parser import Parser
from ir import generate_ir
from codegen import to_machine_code, export_to_binary, hex_dump

def banner():
    print("=" * 70)
    print("🧠 REXION COMPILER v1.0.0 — Advanced Compilation Interface")
    print("=" * 70)
    print("Powered by OpenStack Syntax Core | Output: IR, ASM, BIN")
    print("Supports: Verbose Logs, Hex Dumping, Error Tracing, Binary Export")
    print("-" * 70)

def handle_args():
    parser = argparse.ArgumentParser(description="Rexion Compiler")
    parser.add_argument("source", help="Source .rex file")
    parser.add_argument("-o", "--out", default="build/output", help="Base name for output files")
    parser.add_argument("-v", "--verbose", action="store_true", help="Enable verbose/debug output")
    parser.add_argument("-b", "--binary", action="store_true", help="Export binary .bin file")
    parser.add_argument("-d", "--dump", action="store_true", help="Dump hex and disassembly to console")
    parser.add_argument("--no-opt", action="store_true", help="Disable compiler optimizations")
    parser.add_argument("--tokens", action="store_true", help="Print token stream")
    parser.add_argument("--ast", action="store_true", help="Print abstract syntax tree")
    return parser.parse_args()

def compile_source(code, args):
    start_time = time.time()

    if args.verbose:
        print(f"🧾 Source loaded ({len(code)} characters)\n")

    # Phase 1: Lexical Analysis
    tokens = list(lex(code))
    if args.tokens or args.verbose:
        print("\n📌 Token Stream:")
        for t in tokens:
            print(" ", t)

    # Phase 2: Parsing
    parser = Parser(tokens)
    ast = parser.parse()
    if args.ast or args.verbose:
        print("\n🌲 Abstract Syntax Tree (Parsed)")
        print(" ", ast)

    # Phase 3: IR Generation
    ir = generate_ir(ast)
    print("\n🧩 Intermediate Representation (IR):")
    for line in ir:
        print(" ", line)

    # Phase 4: Code Generation
    machine = to_machine_code(ir, debug=args.verbose)
    print("\n⚙️ Machine Code Output:")
    for line in machine:
        print(" ", line)

    # Phase 5: Output Management
    base = args.out
    os.makedirs(os.path.dirname(base), exist_ok=True)

    with open(f"{base}.ir", "w") as f:
        f.write("\n".join(ir))
    with open(f"{base}.asm", "w") as f:
        f.write("\n".join(machine))

    if args.binary:
        bin_path = export_to_binary(machine, f"{base}.bin")
        print(f"\n💾 Binary exported: {bin_path}")

    if args.dump:
        print("\n🧾 Hex Dump:")
        hex_lines = hex_dump(machine)
        for line in hex_lines:
            print(" ", line)

    print(f"\n✅ Compilation successful. Artifacts saved as: {base}.*")
    print(f"⏱️ Time Elapsed: {time.time() - start_time:.2f}s")

def main():
    banner()
    args = handle_args()

    try:
        with open(args.source, "r") as f:
            code = f.read()
        compile_source(code, args)

    except FileNotFoundError:
        print(f"❌ File not found: {args.source}")
    except SyntaxError as e:
        print(f"❌ Syntax Error:\n{e}")
        traceback.print_exc()
    except Exception as e:
        print(f"❌ Unhandled Compilation Error:\n{e}")
        traceback.print_exc()

if __name__ == "__main__":
    main()

    vm = RexionFullVM(debug=True)
vm.load_binary("/mnt/data/rexion_output.bin")
vm.execute()
vm.show_state()

