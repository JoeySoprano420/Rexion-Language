from lexer import lex

class Parser:
    def __init__(self, tokens):
        self.tokens = list(tokens)
        self.pos = 0

    def peek(self):
        return self.tokens[self.pos] if self.pos < len(self.tokens) else ('EOF', '')

    def peek_next(self):
        if self.pos + 1 < len(self.tokens):
            return self.tokens[self.pos + 1]
        return ('EOF', '')

    def advance(self):
        token = self.peek()
        self.pos += 1
        return token

    def check(self, kind):
        return self.peek()[0] == kind

    def match(self, *kinds):
        if self.peek()[0] in kinds:
            return self.advance()
        return None

    def expect(self, kind, message="Expected token"):
        if self.check(kind):
            return self.advance()
        raise SyntaxError(f"{message}: got {self.peek()}")

    def parse(self):
        ast = []
        while not self.check("EOF"):
            ast.append(self.declaration())
        return ast

    def declaration(self):
        kind, val = self.peek()
        if val == 'func':
            return self.func_decl()
        elif val == 'let':
            return self.var_decl()
        else:
            return self.statement()

    def func_decl(self):
        self.expect('KEYWORD', 'Expected "func"')
        name = self.expect('IDENT', 'Expected function name')[1]
        self.expect('LPAREN')
        params = []
        if not self.check('RPAREN'):
            params.append(self.expect('IDENT')[1])
            while self.match('COMMA'):
                params.append(self.expect('IDENT')[1])
        self.expect('RPAREN')
        body = self.block()
        return ('func_decl', name, params, body)

    def var_decl(self):
        self.expect('KEYWORD', 'Expected "let"')
        name = self.expect('IDENT')[1]
        self.expect('ASSIGN')
        expr = self.expression()
        self.expect('SEMICOLON')
        return ('var_decl', name, expr)

    def statement(self):
        kind, val = self.peek()
        if val == 'if':
            return self.if_stmt()
        elif val == 'while':
            return self.while_stmt()
        elif val == 'return':
            return self.return_stmt()
        elif val == 'print':
            return self.print_stmt()
        elif self.check('LBRACE'):
            return self.block()
        else:
            return self.expr_stmt()

    def if_stmt(self):
        self.expect('KEYWORD', 'Expected "if"')
        self.expect('LPAREN')
        cond = self.expression()
        self.expect('RPAREN')
        then_branch = self.block()
        else_branch = None
        if self.peek()[1] == 'else':
            self.advance()
            else_branch = self.block()
        return ('if', cond, then_branch, else_branch)

    def while_stmt(self):
        self.expect('KEYWORD', 'Expected "while"')
        self.expect('LPAREN')
        cond = self.expression()
        self.expect('RPAREN')
        body = self.block()
        return ('while', cond, body)

    def return_stmt(self):
        self.expect('KEYWORD', 'Expected "return"')
        expr = self.expression()
        self.expect('SEMICOLON')
        return ('return', expr)

    def print_stmt(self):
        self.expect('KEYWORD', 'Expected "print"')
        expr = self.expression()
        self.expect('SEMICOLON')
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
        self.expect('SEMICOLON')
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
            op = self.tokens[self.pos - 1][1]
            right = self.comparison()
            expr = ('binary', expr, op, right)
        return expr

    def comparison(self):
        expr = self.term()
        while self.match('LT', 'GT', 'LE', 'GE'):
            op = self.tokens[self.pos - 1][1]
            right = self.term()
            expr = ('binary', expr, op, right)
        return expr

    def term(self):
        expr = self.factor()
        while self.match('PLUS', 'MINUS'):
            op = self.tokens[self.pos - 1][1]
            right = self.factor()
            expr = ('binary', expr, op, right)
        return expr

    def factor(self):
        expr = self.unary()
        while self.match('STAR', 'SLASH'):
            op = self.tokens[self.pos - 1][1]
            right = self.unary()
            expr = ('binary', expr, op, right)
        return expr

    def unary(self):
        if self.match('BANG', 'MINUS'):
            op = self.tokens[self.pos - 1][1]
            right = self.unary()
            return ('unary', op, right)
        return self.primary()

    def primary(self):
        kind, val = self.advance()
        if kind == 'NUMBER':
            return ('literal', float(val) if '.' in val else int(val))
        elif kind == 'STRING':
            return ('literal', val)
        elif kind == 'IDENT':
            if self.check('LPAREN'):
                return self.finish_call(('var', val))
            return ('var', val)
        elif kind == 'LPAREN':
            expr = self.expression()
            self.expect('RPAREN')
            return expr
        else:
            raise SyntaxError(f"Unexpected token: {kind} ({val})")

    def finish_call(self, callee):
        self.expect('LPAREN')
        args = []
        if not self.check('RPAREN'):
            args.append(self.expression())
            while self.match('COMMA'):
                args.append(self.expression())
        self.expect('RPAREN')
        return ('call', callee, args)
