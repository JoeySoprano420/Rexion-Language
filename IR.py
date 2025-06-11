def generate_ir(ast):
    ir = []
    for node in ast:
        kind = node[0]

        if kind == 'print':
            ir.append(f"[PRINT] -> {flatten_expr(node[1])}")

        elif kind == 'assign':
            target = node[1]
            value = flatten_expr(node[2])
            ir.append(f"[ASSIGN] {target} = {value}")

        elif kind == 'var_decl':
            target = node[1]
            value = flatten_expr(node[2])
            ir.append(f"[DECLARE] {target} = {value}")

        elif kind == 'call':
            func = flatten_expr(node[1])
            args = ', '.join(flatten_expr(arg) for arg in node[2])
            ir.append(f"[CALL] {func}({args})")

        elif kind == 'func_decl':
            func_name = node[1]
            params = node[2]
            ir.append(f"[FUNC_DECL] {func_name}({', '.join(params)})")
            ir += generate_ir(node[3][1])  # node[3] is ('block', [...])
            ir.append("[END_FUNC]")

        elif kind == 'return':
            ir.append(f"[RETURN] {flatten_expr(node[1])}")

        elif kind == 'if':
            cond = flatten_expr(node[1])
            ir.append(f"[IF] {cond}")
            ir += generate_ir(node[2][1])  # then block
            if node[3]:
                ir.append(f"[ELSE]")
                ir += generate_ir(node[3][1])  # else block
            ir.append("[END_IF]")

        elif kind == 'while':
            cond = flatten_expr(node[1])
            ir.append(f"[WHILE] {cond}")
            ir += generate_ir(node[2][1])  # body block
            ir.append("[END_WHILE]")

        elif kind == 'expr_stmt':
            expr_code = flatten_expr(node[1])
            ir.append(f"[EXPR] {expr_code}")

        elif kind == 'macro_decl':
            name = node[1]
            ir.append(f"[MACRO_DECL] {name}")
            ir += generate_ir(node[2])
            ir.append("[END_MACRO]")

        elif kind == 'block':
            ir.append("[BLOCK_START]")
            ir += generate_ir(node[1])
            ir.append("[BLOCK_END]")

        else:
            ir.append(f"[UNKNOWN_NODE] {kind}")
    return ir

def flatten_expr(expr):
    """Converts expression tuples to string IR representations."""
    kind = expr[0]
    if kind == 'literal':
        return str(expr[1])
    elif kind == 'var':
        return expr[1]
    elif kind == 'binary':
        return f"({flatten_expr(expr[1])} {expr[2]} {flatten_expr(expr[3])})"
    elif kind == 'unary':
        return f"({expr[1]} {flatten_expr(expr[2])})"
    elif kind == 'logical':
        return f"({flatten_expr(expr[1])} {expr[2]} {flatten_expr(expr[3])})"
    elif kind == 'call':
        callee = flatten_expr(expr[1])
        args = ', '.join(flatten_expr(arg) for arg in expr[2])
        return f"{callee}({args})"
    else:
        return f"<EXPR {kind}>"

