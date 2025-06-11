from lexer import lex

class Parser:
    def __init__(self, tokens):
        self.tokens = list(tokens)
        self.pos = 0

    def peek(self):
        return self.tokens[self.pos] if self.pos < len(self.tokens) else ('EOF', '')

    def advance(self):
        token = self.peek()
        self.pos += 1
        return token

    def parse(self):
        ast = []
        while self.peek()[0] != 'EOF':
            ast.append(self.statement())
        return ast

    def statement(self):
        kind, val = self.peek()
        if val == 'func':
            return self.func_decl()
        elif val == 'print':
            return self.print_stmt()
        elif kind == 'IDENT':
            return self.assign_or_call()
        else:
            raise SyntaxError(f'Unknown start of statement: {val}')

    def func_decl(self):
        self.advance()  # 'func'
        name = self.advance()[1]
        self.advance()  # '('
        params = []
        if self.peek()[0] != 'RPAREN':
            params.append(self.advance()[1])
            while self.peek()[0] == 'COMMA':
                self.advance()
                params.append(self.advance()[1])
        self.advance()  # ')'
        self.advance()  # '{'
        body = []
        while self.peek()[0] != 'RBRACE':
            body.append(self.statement())
        self.advance()  # '}'
        return ('func_decl', name, params, body)

    def assign_or_call(self):
        name = self.advance()[1]
        if self.peek()[0] == 'ASSIGN':
            self.advance()
            expr = self.expression()
            self.advance()  # ';'
            return ('assign', name, expr)
        elif self.peek()[0] == 'LPAREN':
            self.advance()
            args = []
            if self.peek()[0] != 'RPAREN':
                args.append(self.expression())
                while self.peek()[0] == 'COMMA':
                    self.advance()
                    args.append(self.expression())
            self.advance()  # ')'
            self.advance()  # ';'
            return ('call', name, args)

    def print_stmt(self):
        self.advance()
        expr = self.expression()
        self.advance()  # ';'
        return ('print', expr)

    def expression(self):
        # Simplified expression: just return next value
        token = self.advance()
        return ('literal', token[1])
