import re
import struct

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

def export_to_asm(ir_lines, filename="output.asm"):

    Converts IR lines into a simple assembly-style representation.
    Writes to an .asm file.
    
    with open(filename, "w") as f:
        f.write("; Auto-generated Rexion Assembly\n\n")
        label_counter = 0
        indent = "  "

        for line in ir_lines:
            if line.startswith("[FUNC_DECL]"):
                name = line.split()[1].split('(')[0]
                f.write(f"{name}:\n")
            elif line.startswith("[END_FUNC]"):
                f.write(f"{indent}RET\n")
            elif line.startswith("[DECLARE]") or line.startswith("[ASSIGN]"):
                parts = line.split()
                var = parts[1]
                expr = " ".join(parts[3:])
                f.write(f"{indent}MOV {var}, {expr}\n")
            elif line.startswith("[PRINT]"):
                value = line.split("->")[1].strip()
                f.write(f"{indent}OUT {value}\n")
            elif line.startswith("[CALL]"):
                name = line.split()[1].split('(')[0]
                f.write(f"{indent}CALL {name}\n")
            elif line.startswith("[RETURN]"):
                value = line.split()[1]
                f.write(f"{indent}MOV RETVAL, {value}\n")
                f.write(f"{indent}RET\n")
            elif line.startswith("[IF]"):
                cond = " ".join(line.split()[1:])
                label = f"L{label_counter}"
                label_counter += 1
                f.write(f"{indent}CMP {cond}\n")
                f.write(f"{indent}JZ {label}_else\n")
            elif line == "[ELSE]":
                f.write(f"{indent}JMP {label}_endif\n")
                f.write(f"{label}_else:\n")
            elif line == "[END_IF]":
                f.write(f"{label}_endif:\n")
            elif line.startswith("[WHILE]"):
                cond = " ".join(line.split()[1:])
                f.write(f"{label}_while:\n")
                f.write(f"{indent}CMP {cond}\n")
                f.write(f"{indent}JZ {label}_endwhile\n")
            elif line == "[END_WHILE]":
                f.write(f"{indent}JMP {label}_while\n")
                f.write(f"{label}_endwhile:\n")
            elif line.startswith("[EXPR]"):
                expr = line.split(" ", 1)[1]
                f.write(f"{indent}EVAL {expr}\n")
            elif line.startswith("[BLOCK_START]") or line.startswith("[BLOCK_END]"):
                continue  # Blocks are implicit in .asm output
            elif line.startswith("[MACRO_DECL]"):
                name = line.split()[1]
                f.write(f"{name}_macro:\n")
            elif line.startswith("[END_MACRO]"):
                f.write(f"{indent}RET\n")
            else:
                f.write(f"; Unknown or unhandled IR: {line}\n")

    print(f"✅ Assembly written to {filename}")

class LabelTracker:
    def __init__(self):
        self.counter = 0
        self.stack = []

    def new(self, base):
        label = f"{base.upper()}_{self.counter}"
        self.counter += 1
        return label

    def push(self, start, end):
        self.stack.append((start, end))

    def pop(self):
        return self.stack.pop()

def export_to_asm_and_bin(ir_lines, asm_filename="rexion_output.asm", bin_filename="rexion_output.bin"):
    """
    Converts IR lines into both a readable assembly file and a simplistic binary format.
    """
    label_tracker = LabelTracker()
    asm_lines = []
    binary = bytearray()

    asm_lines.append("; Auto-generated Rexion Assembly\n")

    for line in ir_lines:
        if line.startswith("[FUNC_DECL]"):
            name = line.split()[1].split('(')[0]
            asm_lines.append(f"{name}:")
        elif line.startswith("[END_FUNC]"):
            asm_lines.append("  RET")
            binary.extend(b"\xFF")
        elif line.startswith("[DECLARE]") or line.startswith("[ASSIGN]"):
            parts = line.split()
            var = parts[1]
            expr = " ".join(parts[3:])
            asm_lines.append(f"  MOV {var}, {expr}")
            binary.extend(b"\x10")  # MOV opcode
        elif line.startswith("[PRINT]"):
            value = line.split("->")[1].strip()
            asm_lines.append(f"  OUT {value}")
            binary.extend(b"\x20")  # OUT opcode
        elif line.startswith("[CALL]"):
            name = line.split()[1].split('(')[0]
            asm_lines.append(f"  CALL {name}")
            binary.extend(b"\x30")  # CALL opcode
        elif line.startswith("[RETURN]"):
            value = line.split()[1]
            asm_lines.append(f"  MOV RETVAL, {value}")
            asm_lines.append("  RET")
            binary.extend(b"\x40")
        elif line.startswith("[IF]"):
            cond = " ".join(line.split()[1:])
            else_label = label_tracker.new("else")
            end_label = label_tracker.new("endif")
            label_tracker.push(else_label, end_label)
            asm_lines.append(f"  CMP {cond}")
            asm_lines.append(f"  JZ {else_label}")
            binary.extend(b"\x50")  # IF block enter
        elif line == "[ELSE]":
            else_label, end_label = label_tracker.stack[-1]
            asm_lines.append(f"  JMP {end_label}")
            asm_lines.append(f"{else_label}:")
            binary.extend(b"\x51")
        elif line == "[END_IF]":
            else_label, end_label = label_tracker.pop()
            asm_lines.append(f"{end_label}:")
            binary.extend(b"\x52")
        elif line.startswith("[WHILE]"):
            cond = " ".join(line.split()[1:])
            start_label = label_tracker.new("while_start")
            end_label = label_tracker.new("while_end")
            label_tracker.push(start_label, end_label)
            asm_lines.append(f"{start_label}:")
            asm_lines.append(f"  CMP {cond}")
            asm_lines.append(f"  JZ {end_label}")
            binary.extend(b"\x60")
        elif line == "[END_WHILE]":
            start_label, end_label = label_tracker.pop()
            asm_lines.append(f"  JMP {start_label}")
            asm_lines.append(f"{end_label}:")
            binary.extend(b"\x61")
        elif line.startswith("[EXPR]"):
            expr = line.split(" ", 1)[1]
            asm_lines.append(f"  EVAL {expr}")
            binary.extend(b"\x70")
        elif line.startswith("[BLOCK_START]") or line.startswith("[BLOCK_END]"):
            continue
        else:
            asm_lines.append(f"; Unhandled IR: {line}")

    # Write to .asm
    with open(asm_filename, "w") as f:
        f.write("\n".join(asm_lines))
    print(f"✅ Assembly written to {asm_filename}")

    # Write to .bin
    with open(bin_filename, "wb") as f:
        f.write(binary)
    print(f"✅ Binary written to {bin_filename} ({len(binary)} bytes)")
