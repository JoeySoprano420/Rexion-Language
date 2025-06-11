import sys
from lexer import lex
from parser import Parser
from ir import generate_ir
from codegen import to_machine_code

def main(file):
    with open(file) as f:
        code = f.read()
    tokens = list(lex(code))
    parser = Parser(tokens)
    ast = parser.parse()
    ir = generate_ir(ast)
    machine = to_machine_code(ir)
    
    print("\nIR Output:")
    for line in ir:
        print(" ", line)
    print("\nMachine Code:")
    for line in machine:
        print(" ", line)

if __name__ == "__main__":
    main(sys.argv[1])
