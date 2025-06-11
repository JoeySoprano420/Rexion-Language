def generate_ir(ast):
    ir = []
    for node in ast:
        if node[0] == 'print':
            ir.append(f"[PRINT] -> {node[1][1]}")
        elif node[0] == 'assign':
            ir.append(f"[ASSIGN] {node[1]} = {node[2][1]}")
        elif node[0] == 'call':
            args = ', '.join(arg[1] for arg in node[2])
            ir.append(f"[CALL] {node[1]}({args})")
        elif node[0] == 'func_decl':
            ir.append(f"[FUNC_DECL] {node[1]}({', '.join(node[2])})")
            for stmt in node[3]:
                ir += generate_ir([stmt])
            ir.append("[END_FUNC]")
    return ir
