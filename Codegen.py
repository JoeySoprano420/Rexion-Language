import struct

def to_machine_code(ir, debug=False):
    machine = []
    labels = {}
    pending_jumps = []
    register_map = {f"R{i}": i for i in range(8)}
    stack = []
    stack_frame = {}

    def log(msg):
        if debug:
            print(f"[DEBUG] {msg}")

    # First pass: collect all labels and function declarations
    addr = 0
    for instr in ir:
        if instr.startswith("[FUNC_DECL]"):
            label = instr.split()[1]
            labels[label] = addr
        elif instr.startswith("[LABEL]"):
            label = instr.split()[1]
            labels[label] = addr
        addr += 1

    # Second pass: translate IR into machine instructions
    for idx, instr in enumerate(ir):
        instr = instr.strip()

        if instr.startswith("[PRINT]"):
            val = instr.split("->")[1].strip()
            encoded_val = " ".join(f"{ord(c):02X}" for c in val)
            machine.append(f"OUT {len(val):02X} {encoded_val}")
            log(f"PRINT '{val}'")

        elif instr.startswith("[ASSIGN]"):
            parts = instr.replace("[ASSIGN]", "").strip().split("->")
            dest = parts[0].strip()
            value = parts[1].strip()
            try:
                val_int = int(value)
                machine.append(f"MOV {dest}, {val_int}")
                if dest in register_map:
                    stack_frame[dest] = val_int
            except ValueError:
                machine.append(f"MOV {dest}, {value}")
            log(f"ASSIGN {dest} = {value}")

        elif instr.startswith("[CALL]"):
            func_name = instr.split()[1]
            addr = labels.get(func_name, None)
            if addr is not None:
                machine.append(f"CALL {addr:04X}")
            else:
                pending_jumps.append((idx, func_name))
                machine.append(f"CALL {func_name}")
            log(f"CALL {func_name}")

        elif instr.startswith("[RET]"):
            machine.append("RET")
            log("RETURN")

        elif instr.startswith("[PUSH]"):
            val = instr.split()[1]
            machine.append(f"PUSH {val}")
            stack.append(val)
            log(f"PUSH {val}")

        elif instr.startswith("[POP]"):
            reg = instr.split()[1]
            machine.append(f"POP {reg}")
            if stack:
                stack_frame[reg] = stack.pop()
            log(f"POP into {reg}")

        elif instr.startswith("[FUNC_DECL]"):
            name = instr.split()[1]
            machine.append(f"; FUNC {name}")
            log(f"FUNC_DECL {name}")

        elif instr.startswith("[END_FUNC]"):
            machine.append("; END FUNC")
            log("END_FUNC")

        elif instr.startswith("[LABEL]"):
            label = instr.split()[1]
            machine.append(f"{label}:")
            log(f"LABEL {label}")

        elif instr.startswith("[JMP]"):
            label = instr.split()[1]
            if label in labels:
                addr = labels[label]
                machine.append(f"JMP {addr:04X}")
            else:
                pending_jumps.append((idx, label))
                machine.append(f"JMP {label}")
            log(f"JMP to {label}")

        elif instr.startswith("[CMP]"):
            a, b = instr.replace("[CMP]", "").split(",")
            machine.append(f"CMP {a.strip()}, {b.strip()}")
            log(f"CMP {a.strip()} {b.strip()}")

        elif instr.startswith("[JZ]"):
            label = instr.split()[1]
            if label in labels:
                addr = labels[label]
                machine.append(f"JZ {addr:04X}")
            else:
                pending_jumps.append((idx, label))
                machine.append(f"JZ {label}")
            log(f"JZ {label}")

        elif instr.startswith("[JNZ]"):
            label = instr.split()[1]
            if label in labels:
                addr = labels[label]
                machine.append(f"JNZ {addr:04X}")
            else:
                pending_jumps.append((idx, label))
                machine.append(f"JNZ {label}")
            log(f"JNZ {label}")

        elif instr.startswith("[HALT]"):
            machine.append("HLT")
            log("HALT")

        elif instr.startswith("[NOP]"):
            machine.append("NOP")
            log("NOP")

        else:
            machine.append(f"; Unrecognized: {instr}")
            log(f"UNKNOWN {instr}")

    # Resolve pending jump placeholders
    for idx, label in pending_jumps:
        if label in labels:
            addr = labels[label]
            machine[idx] = machine[idx].replace(label, f"{addr:04X}")

    return machine

def export_to_binary(machine_code, filename="output.bin"):
    binary = bytearray()
    for line in machine_code:
        parts = line.split()
        for part in parts:
            if part.startswith(";") or part.endswith(":"):
                continue
            try:
                val = int(part, 16)
                binary.append(val)
            except ValueError:
                continue  # Ignore non-hex or symbolic parts
    with open(f"/mnt/data/{filename}", "wb") as f:
        f.write(binary)
    return f"/mnt/data/{filename}"

# --- EXAMPLE TEST CASE ---
if __name__ == "__main__":
    ir_code = [
        "[FUNC_DECL] main",
        "[ASSIGN] R1 -> 5",
        "[ASSIGN] R2 -> 10",
        "[PRINT] -> Hello",
        "[PUSH] R1",
        "[POP] R3",
        "[LABEL] loop_start",
        "[CMP] R1, R2",
        "[JZ] end",
        "[JMP] loop_start",
        "[LABEL] end",
        "[RET]",
        "[END_FUNC]"
    ]

    mc = to_machine_code(ir_code, debug=True)
    print("\n".join(mc))

    binary_path = export_to_binary(mc)
    print(f"\n✅ Binary exported to: {binary_path}")
