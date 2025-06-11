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

def disassemble_bin(filename="rexion_output.bin"):
    opcode_map = {
        0x10: "MOV",
        0x20: "OUT",
        0x30: "CALL",
        0x40: "RETURN",
        0x50: "IF_START",
        0x51: "ELSE",
        0x52: "END_IF",
        0x60: "WHILE_START",
        0x61: "END_WHILE",
        0x70: "EVAL",
        0xFF: "FUNC_END"
    }

    print(f"\n🧪 Disassembled .bin from {filename}:\n")
    try:
        with open(filename, "rb") as f:
            binary = f.read()
            for i, byte in enumerate(binary):
                opcode = byte
                if opcode in opcode_map:
                    print(f"{i:04X}: {opcode_map[opcode]}")
                else:
                    print(f"{i:04X}: UNKNOWN OPCODE 0x{opcode:02X} ❌")
    except FileNotFoundError:
        print(f"⚠️ File {filename} not found.")
    except Exception as e:
        print(f"❌ Error disassembling: {e}")

def verify_bin(filename="rexion_output.bin"):
    allowed_opcodes = {
        0x10, 0x20, 0x30, 0x40,
        0x50, 0x51, 0x52,
        0x60, 0x61,
        0x70, 0xFF
    }
    print(f"\n🔒 Verifying opcodes in {filename}:\n")
    try:
        with open(filename, "rb") as f:
            binary = f.read()
            for i, byte in enumerate(binary):
                if byte not in allowed_opcodes:
                    print(f"❌ Illegal opcode at byte {i}: 0x{byte:02X}")
                    return False
        print("✅ All opcodes valid.")
        return True
    except FileNotFoundError:
        print(f"⚠️ File {filename} not found.")
        return False
    except Exception as e:
        print(f"❌ Error verifying: {e}")
        return False

disassemble_bin("rexion_output.bin")

verify_bin("rexion_output.bin")

class RexionVM:
    def __init__(self, debug=False):
        self.registers = {}
        self.retval = None
        self.output = []
        self.debug = debug
        self.pc = 0
        self.binary = []
        self.opcode_map = {
            0x10: self.op_mov,
            0x20: self.op_out,
            0x30: self.op_call,
            0x40: self.op_return,
            0x50: self.op_if_start,
            0x51: self.op_else,
            0x52: self.op_end_if,
            0x60: self.op_while_start,
            0x61: self.op_end_while,
            0x70: self.op_eval,
            0xFF: self.op_func_end
        }

    def load_binary(self, filename):
        with open(filename, "rb") as f:
            self.binary = list(f.read())

    def execute(self):
        while self.pc < len(self.binary):
            opcode = self.binary[self.pc]
            self.pc += 1
            if opcode in self.opcode_map:
                self.opcode_map[opcode]()
            else:
                raise ValueError(f"Illegal opcode 0x{opcode:02X} at PC={self.pc - 1}")

    def op_mov(self):
        # For demonstration, simulate MOV with placeholder
        self.registers['x'] = 5
        if self.debug:
            print(f"[MOV] x = 5")

    def op_out(self):
        val = self.registers.get('x', 'undefined')
        self.output.append(val)
        if self.debug:
            print(f"[OUT] {val}")

    def op_call(self):
        if self.debug:
            print("[CALL] Simulated function call")

    def op_return(self):
        self.retval = self.registers.get('x', None)
        if self.debug:
            print(f"[RETURN] retval = {self.retval}")

    def op_if_start(self):
        if self.debug:
            print("[IF_START] condition evaluated (simulated)")

    def op_else(self):
        if self.debug:
            print("[ELSE] jump to else branch")

    def op_end_if(self):
        if self.debug:
            print("[END_IF] end of if-else block")

    def op_while_start(self):
        if self.debug:
            print("[WHILE_START] entering while loop (simulated)")

    def op_end_while(self):
        if self.debug:
            print("[END_WHILE] loop complete (simulated)")

    def op_eval(self):
        if self.debug:
            print("[EVAL] expression evaluated (simulated)")

    def op_func_end(self):
        if self.debug:
            print("[FUNC_END] function end")

    def show_output(self):
        print("\n💻 Program Output:")
        for val in self.output:
            print(" ", val)

# Example usage:
# vm = RexionVM(debug=True)
# vm.load_binary("rexion_output.bin")
# vm.execute()
# vm.show_output()

vm = RexionVM(debug=True)
vm.load_binary("rexion_output.bin")
vm.execute()
vm.show_output()

class RexionFullVM:
    def __init__(self, debug=False):
        self.registers = {f"R{i}": 0 for i in range(8)}
        self.stack = []
        self.memory = {}
        self.pc = 0
        self.binary = []
        self.output = []
        self.debug = debug
        self.labels = {}
        self.loop_stack = []
        self.running = True

        self.opcode_map = {
            0x10: self.op_mov,
            0x11: self.op_add,
            0x12: self.op_sub,
            0x13: self.op_mul,
            0x14: self.op_div,
            0x20: self.op_out,
            0x30: self.op_call,
            0x40: self.op_return,
            0x50: self.op_if_start,
            0x51: self.op_else,
            0x52: self.op_end_if,
            0x60: self.op_while_start,
            0x61: self.op_end_while,
            0xFF: self.op_func_end
        }

    def load_binary(self, filename):
        with open(filename, "rb") as f:
            self.binary = list(f.read())

    def execute(self):
        while self.pc < len(self.binary) and self.running:
            opcode = self.binary[self.pc]
            self.pc += 1
            if opcode in self.opcode_map:
                self.opcode_map[opcode]()
            else:
                raise ValueError(f"Illegal opcode 0x{opcode:02X} at PC={self.pc - 1}")

    def fetch_value(self):
        val = self.binary[self.pc]
        self.pc += 1
        return val

    def op_mov(self):
        reg = f"R{self.fetch_value()}"
        val = self.fetch_value()
        self.registers[reg] = val
        if self.debug:
            print(f"[MOV] {reg} = {val}")

    def op_add(self):
        reg = f"R{self.fetch_value()}"
        a = self.fetch_value()
        b = self.fetch_value()
        self.registers[reg] = a + b
        if self.debug:
            print(f"[ADD] {reg} = {a} + {b}")

    def op_sub(self):
        reg = f"R{self.fetch_value()}"
        a = self.fetch_value()
        b = self.fetch_value()
        self.registers[reg] = a - b
        if self.debug:
            print(f"[SUB] {reg} = {a} - {b}")

    def op_mul(self):
        reg = f"R{self.fetch_value()}"
        a = self.fetch_value()
        b = self.fetch_value()
        self.registers[reg] = a * b
        if self.debug:
            print(f"[MUL] {reg} = {a} * {b}")

    def op_div(self):
        reg = f"R{self.fetch_value()}"
        a = self.fetch_value()
        b = self.fetch_value()
        self.registers[reg] = a // b if b != 0 else 0
        if self.debug:
            print(f"[DIV] {reg} = {a} / {b}")

    def op_out(self):
        reg = f"R{self.fetch_value()}"
        val = self.registers.get(reg, 0)
        self.output.append(val)
        if self.debug:
            print(f"[OUT] {reg} => {val}")

    def op_call(self):
        addr = self.fetch_value()
        self.stack.append(self.pc)
        self.pc = addr
        if self.debug:
            print(f"[CALL] Jump to {addr}")

    def op_return(self):
        self.pc = self.stack.pop() if self.stack else len(self.binary)
        if self.debug:
            print(f"[RETURN] PC set to {self.pc}")

    def op_if_start(self):
        reg = f"R{self.fetch_value()}"
        val = self.registers.get(reg, 0)
        skip_bytes = self.fetch_value()
        if self.debug:
            print(f"[IF] R({reg}) == {val}, skip {skip_bytes} if false")
        if val == 0:
            self.pc += skip_bytes

    def op_else(self):
        skip_bytes = self.fetch_value()
        if self.debug:
            print(f"[ELSE] Skipping {skip_bytes} bytes")
        self.pc += skip_bytes

    def op_end_if(self):
        if self.debug:
            print(f"[END_IF]")

    def op_while_start(self):
        cond_reg = f"R{self.fetch_value()}"
        skip_bytes = self.fetch_value()
        loop_start = self.pc
        val = self.registers.get(cond_reg, 0)
        if val == 0:
            self.pc += skip_bytes
        else:
            self.loop_stack.append((cond_reg, loop_start, skip_bytes))
        if self.debug:
            print(f"[WHILE] R({cond_reg}) == {val}")

    def op_end_while(self):
        if self.loop_stack:
            cond_reg, loop_start, skip_bytes = self.loop_stack[-1]
            val = self.registers.get(cond_reg, 0)
            if val != 0:
                self.pc = loop_start
            else:
                self.loop_stack.pop()
        if self.debug:
            print(f"[END_WHILE]")

    def op_func_end(self):
        self.running = False
        if self.debug:
            print(f"[FUNC_END]")

    def show_state(self):
        print("\n🧾 Registers:")
        for reg, val in self.registers.items():
            print(f"  {reg} = {val}")
        print("\n🧾 Stack:")
        print(" ", self.stack)
        print("\n💻 Output:")
        for val in self.output:
            print(" ", val)

# Example usage:
# vm = RexionFullVM(debug=True)
# vm.load_binary("rexion_output.bin")
# vm.execute()
# vm.show_state()

import struct

INSTR = {
    "MOV":   0x10,   # MOV reg, value
    "ADD":   0x11,   # ADD reg, a, b
    "SUB":   0x12,   # SUB reg, a, b
    "MUL":   0x13,   # MUL reg, a, b
    "DIV":   0x14,   # DIV reg, a, b
    "OUT":   0x20,   # OUT reg
    "CALL":  0x30,   # CALL addr
    "RETURN":0x40,   # RETURN
    "IF":    0x50,   # IF reg, skip_bytes
    "ELSE":  0x51,   # ELSE skip_bytes
    "END_IF":0x52,   # END_IF
    "WHILE": 0x60,   # WHILE reg, skip_bytes
    "END_WHILE":0x61,# END_WHILE
    "FUNC_END": 0xFF # FUNC_END
}

REGISTERS = {f"R{i}": i for i in range(8)}

def reg_num(var):
    """Map variable name to register number, defaulting to R0 if not found."""
    return REGISTERS.get(var, 0)

def ir_to_bin(ir, filename="rexion_output.bin"):
    """Encode IR directly to .bin for RexionFullVM."""
    bytecode = bytearray()
    label_addrs = {}
    patch_table = []
    pc = 0

    # First pass: assign label addresses (for jumps/loops)
    for i, line in enumerate(ir):
        if line.endswith(":"):  # Label (e.g., LOOP_0:)
            label_addrs[line.strip(":")] = pc
        else:
            # Estimate size (every instr here is 1+N bytes)
            if "[ADD]" in line or "[SUB]" in line or "[MUL]" in line or "[DIV]" in line:
                pc += 4  # opcode, reg, a, b
            elif "[MOV]" in line or "[ASSIGN]" in line or "[DECLARE]" in line:
                pc += 3  # opcode, reg, val
            elif "[OUT]" in line or "[PRINT]" in line:
                pc += 2  # opcode, reg
            elif "[IF]" in line or "[ELSE]" in line or "[WHILE]" in line:
                pc += 3  # opcode, reg, skip
            elif "[CALL]" in line:
                pc += 2  # opcode, addr
            elif "[RETURN]" in line or "[FUNC_END]" in line or "[END_WHILE]" in line or "[END_IF]" in line:
                pc += 1
            else:
                pc += 1

    # Second pass: emit bytecode
    for i, line in enumerate(ir):
        tokens = line.replace(',', ' ').split()
        if "[MOV]" in line or "[ASSIGN]" in line or "[DECLARE]" in line:
            # Format: [ASSIGN] x = 5  or [MOV] R1 = 5
            reg = reg_num(tokens[1])
            val = int(tokens[3])
            bytecode += bytes([INSTR["MOV"], reg, val])
        elif "[ADD]" in line or "[SUB]" in line or "[MUL]" in line or "[DIV]" in line:
            op = "ADD" if "[ADD]" in line else "SUB" if "[SUB]" in line else "MUL" if "[MUL]" in line else "DIV"
            reg = reg_num(tokens[1])
            a = int(tokens[3])
            b = int(tokens[5])
            bytecode += bytes([INSTR[op], reg, a, b])
        elif "[PRINT]" in line or "[OUT]" in line:
            reg = reg_num(tokens[-1])
            bytecode += bytes([INSTR["OUT"], reg])
        elif "[CALL]" in line:
            addr = 0  # For demo, skip linking real addresses
            bytecode += bytes([INSTR["CALL"], addr])
        elif "[RETURN]" in line:
            bytecode += bytes([INSTR["RETURN"]])
        elif "[IF]" in line:
            reg = reg_num(tokens[1])
            skip = 4  # Placeholder; real skip calculated in patch step
            patch_table.append((len(bytecode) + 2, "IF_SKIP", i))
            bytecode += bytes([INSTR["IF"], reg, skip])
        elif "[ELSE]" in line:
            skip = 4
            patch_table.append((len(bytecode) + 1, "ELSE_SKIP", i))
            bytecode += bytes([INSTR["ELSE"], skip])
        elif "[END_IF]" in line:
            bytecode += bytes([INSTR["END_IF"]])
        elif "[WHILE]" in line:
            reg = reg_num(tokens[1])
            skip = 6
            patch_table.append((len(bytecode) + 2, "WHILE_SKIP", i))
            bytecode += bytes([INSTR["WHILE"], reg, skip])
        elif "[END_WHILE]" in line:
            bytecode += bytes([INSTR["END_WHILE"]])
        elif "[FUNC_END]" in line:
            bytecode += bytes([INSTR["FUNC_END"]])
        elif line.endswith(":"):
            continue  # Label only
        else:
            # Unknown or NOP
            bytecode += b"\x00"

    # (Optional) Patch jump skips if full control flow IR is labeled.

    with open(filename, "wb") as f:
        f.write(bytecode)
    print(f"✅ Compiled .rex directly to {filename} ({len(bytecode)} bytes)")

# Usage:
# ast = Parser(list(lex(source))).parse()
# ir = generate_ir(ast)
# ir_to_bin(ir, filename="rexion_output.bin")

