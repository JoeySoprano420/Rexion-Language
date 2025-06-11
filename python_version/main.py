import sys
import argparse
import traceback
import time
import os
from lexer import lex
from parser import Parser
from ir import generate_ir
from codegen import to_machine_code, export_to_binary, hex_dump

def banner():
    print("=" * 70)
    print("🧠 REXION COMPILER v1.0.0 — Advanced Compilation Interface")
    print("=" * 70)
    print("Powered by OpenStack Syntax Core | Output: IR, ASM, BIN")
    print("Supports: Verbose Logs, Hex Dumping, Error Tracing, Binary Export")
    print("-" * 70)

def handle_args():
    parser = argparse.ArgumentParser(description="Rexion Compiler")
    parser.add_argument("source", help="Source .rex file")
    parser.add_argument("-o", "--out", default="build/output", help="Base name for output files")
    parser.add_argument("-v", "--verbose", action="store_true", help="Enable verbose/debug output")
    parser.add_argument("-b", "--binary", action="store_true", help="Export binary .bin file")
    parser.add_argument("-d", "--dump", action="store_true", help="Dump hex and disassembly to console")
    parser.add_argument("--no-opt", action="store_true", help="Disable compiler optimizations")
    parser.add_argument("--tokens", action="store_true", help="Print token stream")
    parser.add_argument("--ast", action="store_true", help="Print abstract syntax tree")
    return parser.parse_args()

def compile_source(code, args):
    start_time = time.time()

    if args.verbose:
        print(f"🧾 Source loaded ({len(code)} characters)\n")

    # Phase 1: Lexical Analysis
    tokens = list(lex(code))
    if args.tokens or args.verbose:
        print("\n📌 Token Stream:")
        for t in tokens:
            print(" ", t)

    # Phase 2: Parsing
    parser = Parser(tokens)
    ast = parser.parse()
    if args.ast or args.verbose:
        print("\n🌲 Abstract Syntax Tree (Parsed)")
        print(" ", ast)

    # Phase 3: IR Generation
    ir = generate_ir(ast)
    print("\n🧩 Intermediate Representation (IR):")
    for line in ir:
        print(" ", line)

    # Phase 4: Code Generation
    machine = to_machine_code(ir, debug=args.verbose)
    print("\n⚙️ Machine Code Output:")
    for line in machine:
        print(" ", line)

    # Phase 5: Output Management
    base = args.out
    os.makedirs(os.path.dirname(base), exist_ok=True)

    with open(f"{base}.ir", "w") as f:
        f.write("\n".join(ir))
    with open(f"{base}.asm", "w") as f:
        f.write("\n".join(machine))

    if args.binary:
        bin_path = export_to_binary(machine, f"{base}.bin")
        print(f"\n💾 Binary exported: {bin_path}")

    if args.dump:
        print("\n🧾 Hex Dump:")
        hex_lines = hex_dump(machine)
        for line in hex_lines:
            print(" ", line)

    print(f"\n✅ Compilation successful. Artifacts saved as: {base}.*")
    print(f"⏱️ Time Elapsed: {time.time() - start_time:.2f}s")

def main():
    banner()
    args = handle_args()

    try:
        with open(args.source, "r") as f:
            code = f.read()
        compile_source(code, args)

    except FileNotFoundError:
        print(f"❌ File not found: {args.source}")
    except SyntaxError as e:
        print(f"❌ Syntax Error:\n{e}")
        traceback.print_exc()
    except Exception as e:
        print(f"❌ Unhandled Compilation Error:\n{e}")
        traceback.print_exc()

if __name__ == "__main__":
    main()
