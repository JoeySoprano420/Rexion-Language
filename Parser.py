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

