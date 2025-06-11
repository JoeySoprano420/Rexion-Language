import re

def generate_ir(ast):
    ir = []
    for node in ast:
        kind = node[0]

        if kind == 'print':
            value = flatten_expr(node[1])
            ir.append(f"[PRINT] -> {value}")

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
    return optimize_ir(verify_ir(ir))


def flatten_expr(expr):
    """Converts expression tuples to string IR representations, with constant folding."""
    kind = expr[0]
    if kind == 'literal':
        return str(expr[1])
    elif kind == 'var':
        return expr[1]
    elif kind == 'binary':
        left = flatten_expr(expr[1])
        op = expr[2]
        right = flatten_expr(expr[3])
        if is_numeric_literal(left) and is_numeric_literal(right):
            return str(eval(f"{left} {op} {right}"))
        return f"({left} {op} {right})"
    elif kind == 'unary':
        op = expr[1]
        right = flatten_expr(expr[2])
        if is_numeric_literal(right):
            return str(eval(f"{op}{right}"))
        return f"({op} {right})"
    elif kind == 'logical':
        left = flatten_expr(expr[1])
        op = expr[2]
        right = flatten_expr(expr[3])
        return f"({left} {op} {right})"
    elif kind == 'call':
        callee = flatten_expr(expr[1])
        args = ', '.join(flatten_expr(arg) for arg in expr[2])
        return f"{callee}({args})"
    return f"<EXPR {kind}>"

def is_numeric_literal(val):
    return re.fullmatch(r'-?\d+(\.\d+)?', val) is not None

def verify_ir(ir_lines):
    """Checks for IR sanity and reports errors or warnings."""
    verified = []
    declared_vars = set()
    for line in ir_lines:
        if line.startswith("[DECLARE]"):
            var = line.split()[1]
            declared_vars.add(var)
        elif line.startswith("[ASSIGN]"):
            var = line.split()[1]
            if var not in declared_vars:
                print(f"⚠️  Warning: Variable '{var}' assigned before declaration")
        elif line.startswith("[PRINT]") or line.startswith("[EXPR]"):
            tokens = re.findall(r'\b\w+\b', line)
            for token in tokens:
                if token.isidentifier() and not token.isupper() and not token.isdigit():
                    if token not in declared_vars and not token.startswith('('):
                        print(f"⚠️  Warning: Use of undeclared variable '{token}' in {line}")
        verified.append(line)
    return verified

def optimize_ir(ir_lines):
    """Applies simple constant folding optimizations."""
    optimized = []
    for line in ir_lines:
        if "[ASSIGN]" in line or "[DECLARE]" in line:
            match = re.search(r'([A-Za-z_][A-Za-z0-9_]*) = (.+)', line)
            if match:
                var, expr = match.groups()
                if is_numeric_literal(expr.strip()):
                    optimized.append(line)
                else:
                    # Check for foldable constant expression
                    try:
                        folded = str(eval(expr))
                        optimized.append(line.split('=')[0] + '= ' + folded)
                        print(f"💡 Optimized: {expr} → {folded}")
                    except:
                        optimized.append(line)
            else:
                optimized.append(line)
        else:
            optimized.append(line)
    return optimized

