# Rexion-Language

---

### ⚙️ **[Rexion Language Definition (Spec v 1.0.1)]**

#### 🔹 **General Identity**

* **Name**: Rexion
* **Type**: High-level, low-syntax, context-inferred systems language
* **Paradigm**: Procedural / Instructional / Flow-Oriented
* **Target**: AOT-compiled machine code with optional JIT and real-time optimization

---

### 🧠 **Semantic Philosophy**

* Python-like **expressiveness**
* C-like **ruleset** and **grammar base**
* Explicit structure like Pascal but with *symbol minimalism*
* Instructional flow clarity like BASIC, but powered by NASM

---

### 🧬 **Core Properties**

| Feature             | Description                                                                    |
| ------------------- | ------------------------------------------------------------------------------ |
| **Grammar**         | C-compliant rules, Python-style indentation optional                           |
| **Semantics**       | Pythonic clarity, English-like simplicity                                      |
| **Syntax**          | Minimalist, NASM-inspired punctuation + symbolic structures                    |
| **Indentation**     | Optional (structured via flow brackets or “->“)                                |
| **Execution**       | AOT (default) + optional JIT (hybrid runtime optimization)                     |
| **Output**          | Raw machine code + IR mapping table                                            |
| **Optimization**    | On-the-fly during JIT, static during AOT                                       |
| **Error Handling**  | Live, context-aware, non-fatal diagnostic pipelines                            |
| **Memory Handling** | Deep, manual + abstracted intrinsics, no GC unless invoked explicitly          |
| **IR Structure**    | Serialized table: “Instruction Flow → Context Reference → Abstraction Mapping” |
| **AST**             | Flow-chart model: “Instruction (NASM-style)” → “Hex” Mapping                   |

---

### 🧾 **{Example Syntax}**

**(Rexion)**

func greet(name)
{
  print "Hello, " + name
}

greet("Violet")


**[Can also be written with NASM-style explicitness:]**

**(Rexion)**

@func greet(name) -> {
  ::print "Hello, " + name
}

@call greet("Violet")


**[Or minimal inline NASM-inspired version:]**

**(Rexion)**

::greet(name)::
  >> print "Hello, " + name
<<

greet "Violet"


---

### 🗃️ **[Rexion Intermediate Representation (RIR) Sample]**


[FUNC_DECL] -> greet
[ARG] -> name
[INSTR] -> print
[CONCAT] -> "Hello, " + name
[END_FUNC]

[CALL] -> greet("Violet")


---

### 🧩 **[Compiler Pipeline]**

1. **Lexer** — Tokenizes minimal syntax into high-level pseudo-instructions
2. **Parser** — Applies C-style AST construction with optional indentation analysis
3. **AST Builder** — Converts code into “Flow-NASM-Hex” representation
4. **IR Serializer** — Stores all instructions in reference-linked mapping table
5. **AOT Compiler** — Converts IR to optimized machine code
6. **JIT Layer (optional)** — Optimizes branches in real time based on live metrics
7. **Execution Engine** — Runs machine code or hybrid code block

---

### 🧠 **[Memory Architecture]**

* Manual allocation (“alloc”, “free”) for low-level dev
* Auto-scope-based cleanup optional (“autofree”)
* Explicit memory zones: “zone stack”, “zone heap”, “zone volatile”
* Intrinsic commands like:

  **(Rexion)**
  
  zone(heap): alloc 256b as buffer
  zone(stack): write buffer "Data"


---

### 📌 **[Minimal Punctuation Rules]**

* "::" for instruction block
* "->" for directional flow / return
* "@" for declarations
* ">> <<" for instruction groupings
* "#" for comment

---




Rexion is a programming language
that uses a UNIVERSAL EXECUTABLE-INSTRUCTION (UEI) ruleset, C grammar, Python semantics, dynamic C syntax with minimal symbols, specified explicit structure, static AOT COMPILATION, JIT execution, on the fly optimizations (real-time), live deep-logic error handling, deep intrinsic memory handling, indents are optional, punctuation is based on simplistic yet powerful NASM and very high-level English, final output is machine code, and ir code is serialized mapping table with direct intrinsic abstraction-based context-aware inference-heavy references, the AST uses flow-chart NASM->hexadecimal.

Create a rule update requiring Define through declarations


AOT

Command prompts + iterative instruction + intuitive detail-oriented 

Stylized intristic sequential modular informative formatted

Procedural cascade flow structure 

Passive correct or skip error handling

Silent recursion and parallelism 

Stack heap allocate assign define wipe clean register allocate deallocate mutex dump store delegate (memory handling) 

Statements arguments strings variables values conditionals truths proofs queries list nest array child branch derivative result group match pattern score stem graph diagram coordinates class object module block segment 

Pop jump mov eax bump

Buffer transition flow control input output command type 

error flag raise throw ignore bypass pass inspect diagnose message minor major rate

Segment limit delimiters Print define immutable mutable request allow deny open close option encrypt decrypt symbol space indentation capitol lower-case bit byte node call function process render export import call routine subroutine let sublet lease hold borrow retrieve collect compile interpret run compare distinguish separate include involve add sub mul div percent negate contrast contradiction highlight recall check break pause continue stop halt resume scope range integer decimal hex octal binary code comment punctuation start end roll unroll assert assume context abstraction automation trigger event async await instance schedule time sequence partition petition performance expect frontend backend hide show annotate comment pseudocode solve record log evaluate audit task read infer compare relationships prioritize hierarchy key map table handle pause prefer macro script line suggest logic gate involve use discard trim optimize release try catch implement generate formal informal make/build run track trace profile cue Que deduce operation reciprocate reverse data purge sanitize revert restore freeze squeeze lookup config serialize seal validate portion numerate enum initiate invert engine truncate ast ir source main frame write prime factor factorial root exponential shape color name namespace age etc

Define-declare-instruct-result (main/usual order of operation)

Intrinsic Grammar

Implicit Syntax

Strong type

Explicit Semantics 

Problem-solving decision-making critical thinking


	1.	✅ Formal .r4 grammar and syntax definition (ANTLR)
	2.	✅ rexionc compiler scaffold (Lexer, Parser, IR, Codegen, CLI)
	3.	✅ Minimal, & Maximum demo program in .r4 and its IR and Machine Code output
	4.	✅ IDE plugin / syntax highlighting file for Visual Studio
	5.	✅ Lexer, Parser, AST, and Rexion IR (RIR)

AOT Compiler, AOT Optimizer, JIT Executor

CLI interface

Makefile, requirements.txt, setup.bat

Standard Library files in .r4 format (e.g., std/io.r4, std/math.r4)

(Lexer — Tokenizes minimal syntax into UNIVERSAL MACHINE-DIALOG-READABLE-INSTRUCTIONS

Parser — Applies C-style AST construction with optional indentation analysis

AST Builder — Converts code into Flow-NASM-Hex representation

IR Serializer — Stores all instructions in reference-linked mapping table

AOT Compiler — Converts IR to optimized machine code

JIT Layer (optional) — Optimizes branches in real time based on live metrics

Execution Engine — Runs machine code or hybrid code block)

UNBIASED WEIGHTS

UNIVERSAL-HIERARCHICAL-EXECUTABLE-INSTRUCTION PARADIGM

DERIVATIVE-PRE-DEFINED TYPE (INSTEAD OF STRICTLY EXPLICIT, DYNAMIC, OR STATIC, ETC)


INHERENT RULSET-REFERENCE INTRENSIC FORENSIC AGRESSIVE CONTEXTALLY ASSUMED ABSTRACTION-ORIENTED INFERENCE - ERROR HANDLING

MACHINE-INTUITIVE MEMORY HANDLING

MANUAL DEEPLY FINE-TUNED SYSTEM ACCESS

MOTIF-ENHANCED SMART-LOGIC SELF-SUPERVISED LEARNING CORE ENGINE, POWERED BY ENHANCED ADVANCED REASONING 

DICTIONARY BUILT-IN, SELF DEFINING THROUGH DEDUCTION, PATTERN-MATCHING, AND ASSOCIATIVE LEARNING

DIRECT MAPPING TO OCTAL CODE, AND REFERNCE TABLES THROUGH LINKERS WITH FALLBACKS TO CALLS, IMPORTS, REQUESTS, ROUTES, LETS, SUBLETS, ROUTINES, SUBROUTINES, AND PIPELINE PROTOCOLS 

SUPER EFFICIENT IN HEAVY LOADS, NUMERICS, MATH, COMPUTE-HEAVY TASKS, EXTENDED SUSTAINED CHALLENGING TASKS, CONSTANTLY CHANGING TASKS, OFTEN-USED TASKS, PERFORMANCE-CRITICAL TASKS, DISTRIBUTED AND CONCURRENT PARRALLEL OPERATIONS, SYSTEM-LEVEL ACCESS, RAPID CLEAN AND ACCURATE ITERATIVE TASKS, 

AUTONOMOUSLY SELF-CLEANS

BEST A PROGRAMMER CAN ASK FOR.

_______________________________________________________________________________________________________________________________________________

UPDATES
_______________________________________________________________________________________________________________________________________________

Here is a complete **overview of the Rexion Language** as it stands, integrating your implemented compiler architecture, IR model, and core language features:

---

# 🧠 **Rexion Language – Full Overview**

Rexion is a **high-level, statically-typed, instruction-oriented programming language** designed for maximum clarity, execution precision, and machine-level traceability. It uses a custom compiler toolchain that includes:

* Lexer → Parser → IR Generator → Codegen → NASM Assembly → `.exe` Output
* Optional AOT + JIT hooks
* Full debug tracing: Tokens, IR, ASM
* Real-time TUI step debugger
* NASM x86\_64 backend (syscalls or `printf` for output)
* Symbol table with float & integer register mapping
* Precision floating-point handling, IR-to-ASM trace, and diagnostics

---

## 🗂️ **Language Structure**

```rexion
define x: int;
x = 5;

func main() {
    print x;
}

class Engine extends Core, Display {
    public func start() {
        print "Starting Engine";
    }
}
```

---

## 🧬 **Grammar & Keywords**

### **Top-Level Constructs**

* `define <name>: <type>;`
* `func <name>() { ... }`
* `class <Name> [extends <Base1, Base2, ...>] { ... }`
* `print <value>;`
* `eval(<expr>);`

### **OOP**

* `public`, `private`, `protected` modifiers
* `new`, `super`, `this` access
* `inherit` / `extends` for multiple inheritance

---

## 🧩 **Core Data Types**

* `int` – standard 64-bit integer
* `float` – IEEE 754 64-bit double
* `string` – immutable UTF-8 string
* *(Type system supports extension in future phases)*

---

## 🎯 **Tokens and Lexical Definitions**

All tokens are defined in `token_type.h` and parsed via `lexer.c`. Custom tokens include:

* `TOKEN_CLASS`, `TOKEN_FUNC`, `TOKEN_DEFINE`, `TOKEN_PUBLIC`, ...
* `TOKEN_ASSIGN (=)`, `TOKEN_SEMI (;)`, `TOKEN_IDENT`, `TOKEN_NUMBER`, `TOKEN_STRING`, etc.

Lexer supports:

* Identifiers
* Strings
* Integer and float literals
* Multi-word class/method tokens
* Error fallback to `TOKEN_UNKNOWN`

---

## ⚙️ **Intermediate Representation (IR)**

IR is generated directly from the AST and output like:

```
[IR] section .code
[IR] entry main
[IR] LOAD R1, 5
[IR] LOAD R2, 3
[IR] ADD R3, R1
[IR] ADD R3, R2
[IR] STORE result, R3
[IR] FLOAT_LOAD F1, 3.14
[IR] FLOAT_ADD F1, F2
[IR] PRINT_FLOAT_SYSCALL F1
[IR] PRINT result
[IR] HALT
```

IR ops include:

* `LOAD`, `STORE`, `ADD`, `SUB`
* `FLOAT_LOAD`, `FLOAT_ADD`, `FLOAT_MUL`
* `PRINT`, `PRINT_FLOAT_PRINTF`, `PRINT_FLOAT_SYSCALL`
* `HALT`

---

## 🧱 **Assembly Codegen**

ASM backend supports:

* x86\_64 NASM syntax
* `mov`, `add`, `syscall`, `call printf`
* Float stack operations via `fld`, `fstp`, `fadd`, etc.
* Registers mapped dynamically via symbol table
* Integer and float conversions (manual syscall string routines)

Includes:

* `int_to_str` for integer display
* `float_to_str` for manual float formatting (with `.precision`)
* Optional `printf` fallback

---

## 🧠 **Debugging Modes**

### 🔍 `--debug-full`

* Dumps: `rexion.tokens`, `rexion.ir`, `rexion.asm`
* IR trace shows every logical step
* Symbol table printout

### 📟 `--debug-tui`

* Interactive step-through of tokens (with type)
* Scroll-style interface
* Useful for language developers, educators, and bytecode tracing

---

## 🔬 **Compiler Architecture**

```
rexionc_main
│
├── lexer.c → tokens[]
├── parser.c → AST
├── ir_codegen.c → IR output
├── asm_codegen.c → rexion.asm
├── tui.c → live token viewer
└── syscall / printf selector → rexion.exe
```

### Compilation Pipeline:

```bash
.rex source → .tokens → .ir → .asm → .o → .exe
```

---

## 🧮 **Symbol Table + Register Allocation**

* 128 symbols max
* Allocates `R1`–`Rn` or `F1`–`Fn` for float vars
* Auto spill-safe design (future: LRU cache or allocator graph)
* Float and int separation ensures correct FPU register use

---

## 📎 **Advanced Features Supported**

* [x] Ray tracing, sculpting, morphing, shading (planned IR extensions)
* [x] Float ops (`FLOAT_ADD`, `FLOAT_LOAD`)
* [x] Dynamic codegen options: syscall or `printf`
* [x] Blueprint for class-based OOP with multiple inheritance
* [x] True compiled `.exe` with NASM + LD

---

## 🚀 **Planned or Extendable Features**

* Conditionals and branching IR (`JMP`, `IFZ`, `CALL`)
* Arrays, structs, classes with memory layout
* Function parameters and stack frames
* Constant folding, dead code elimination
* SIMD extensions, GPU offloading (experimental)
* Built-in physics/audio/multimedia scripting IR ops

---

# Rexion Language – Full Overview

## 🧠 Overview
Rexion is a high-level, statically-typed, instruction-oriented programming language with its own compiler, IR model, assembly backend, and runtime.

## 🗂️ Language Structure
```rexion
define x: int;
x = 5;

func main() {
    print x;
}

class Engine extends Core, Display {
    public func start() {
        print "Starting Engine";
    }
}
```

## 🧬 Grammar & Keywords
- `define`, `func`, `class`, `print`, `eval`
- Visibility: `public`, `private`, `protected`
- Object keywords: `new`, `super`, `this`, `extends`, `inherit`

## 🧩 Core Data Types
- `int`, `float`, `string`

## 🎯 Tokens and Lexical Definitions
Token types like `TOKEN_DEFINE`, `TOKEN_CLASS`, `TOKEN_NUMBER`, `TOKEN_STRING`, etc., with fallback to `TOKEN_UNKNOWN`.

## ⚙️ Intermediate Representation (IR)
```
[IR] section .code
[IR] entry main
[IR] LOAD R1, 5
[IR] ADD R3, R1
...
```

## 🧱 Assembly Codegen
NASM-style assembly generated with:
- Integer and float handling
- Syscall and `printf` output modes
- `int_to_str` and `float_to_str` routines

## 🧠 Debugging Modes
- `--debug-full`: dumps tokens, IR, ASM
- `--debug-tui`: interactive CLI walkthrough

## 🔬 Compiler Architecture
```
rexionc_main → lexer → parser → IR → ASM → .exe
```

## 🧮 Symbol Table
- Supports integer and float symbols
- Dynamic register allocation

## 📎 Advanced Features
- Float ops, class-based OOP, ray tracing, etc.

## 🚀 Planned Features
- Branching, arrays, structs, optimization passes
-----------------------------------------------------------------------------------------------------------------------------------------------
# Rexion Language – Full Overview

## 🧠 Overview
Rexion is a high-level, statically-typed, instruction-oriented programming language with its own compiler, IR model, assembly backend, and runtime.

## 🗂️ Language Structure
```rexion
define x: int;
x = 5;

func main() {
    print x;
}

class Engine extends Core, Display {
    public func start() {
        print "Starting Engine";
    }
}
```

## 🧬 Grammar & Keywords
- `define`, `func`, `class`, `print`, `eval`
- Visibility: `public`, `private`, `protected`
- Object keywords: `new`, `super`, `this`, `extends`, `inherit`

## 🧩 Core Data Types
- `int`, `float`, `string`

## 🎯 Tokens and Lexical Definitions
Token types like `TOKEN_DEFINE`, `TOKEN_CLASS`, `TOKEN_NUMBER`, `TOKEN_STRING`, etc., with fallback to `TOKEN_UNKNOWN`.

## ⚙️ Intermediate Representation (IR)
```
[IR] section .code
[IR] entry main
[IR] LOAD R1, 5
[IR] ADD R3, R1
...
```

## 🧱 Assembly Codegen
NASM-style assembly generated with:
- Integer and float handling
- Syscall and `printf` output modes
- `int_to_str` and `float_to_str` routines

## 🧠 Debugging Modes
- `--debug-full`: dumps tokens, IR, ASM
- `--debug-tui`: interactive CLI walkthrough

## 🔬 Compiler Architecture
```
rexionc_main → lexer → parser → IR → ASM → .exe
```

## 🧮 Symbol Table
- Supports integer and float symbols
- Dynamic register allocation

## 📎 Advanced Features
- Float ops, class-based OOP, ray tracing, etc.

## 🚀 Planned Features
- Branching, arrays, structs, optimization passes
-----------------------------------------------------------------------------------------------------------------------------------------------
