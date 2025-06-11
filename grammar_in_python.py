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
