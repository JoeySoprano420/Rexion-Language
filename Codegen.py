def to_machine_code(ir):
    machine = []
    for instr in ir:
        if instr.startswith("[PRINT]"):
            val = instr.split("->")[1].strip()
            machine.append(f"OUT 0x{len(val):02X} {val}")
        elif instr.startswith("[ASSIGN]"):
            machine.append(f"MOV {instr}")
        elif instr.startswith("[CALL]"):
            machine.append(f"CALL {instr}")
        elif instr.startswith("[FUNC_DECL]"):
            machine.append("; Function Start")
        elif instr.startswith("[END_FUNC]"):
            machine.append("; Function End")
    return machine
