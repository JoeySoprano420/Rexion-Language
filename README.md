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
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UPDATE
________________________________________________________________________________________________

🌌 REXION: Language & Ecosystem Overview (v1.0+)

A Real-Time, Self-Tracing, Macro-Driven Programming Platform

⸻

🧠 What Is Rexion?

Rexion is a hybrid AOT + JIT compiled programming language designed for:
	•	Instruction-oriented expression
	•	Macro-based metaprogramming
	•	Tight NASM x64 alignment
	•	Self-tracing IR pipelines
	•	Real CLI + TUI interactive development

It is an abstracted low-level language with high-level macro expressiveness, designed for:
	•	High-speed builds and live diagnostics
	•	Structured learning curves from C to Assembly
	•	Reusable macro bundles, documentation embedding, and opcode mapping

⸻

🧬 Language Core Principles
	•	UEI Syntax: Universal Executable Instruction structure
	•	C-based grammar + Python-style semantics
	•	Minimal symbolic syntax with expressive keywords
	•	Explicit rule declarations, scopes, modifiers, and types
	•	Instruction-oriented, not object- or functional-first

⸻

📘 Syntax and Semantics Highlights

define x: int;
x = 5;

func greet() {
  print x;
}

class Hero extends Being, Entity {
  public define name: string;
  private func speak() {
    print name;
  }
}

Keywords
	•	define, func, class, extends, public, private, new, super, this, eval, print, return

Symbols & Instructions
	•	= mutable assignment
	•	== immutable
	•	: type spec or fallback
	•	; statement end
	•	|macro| → triggers C.I.A.M.S. block expansion
	•	->, <-, ~>, !, @, $, ^, %, etc. → instruction modifiers
	•	# single-line comment, ** multi-line

⸻

🧱 Compiler Architecture

❱ Pipeline

.r4 source → Lexer → Parser → AST → IR (.rexasm) → ASM (.nasm) → Executable (.exe)

❱ Components
	•	lexer.c / parser.c — tokenization and syntax tree building
	•	ir_codegen.c — IR and register-level abstraction
	•	rexion.asm — NASM x64 generation with optional syscall/printf
	•	rexionc_main.c — full pipeline orchestrator

⸻

🔩 Intermediate Representation (IR)

IR is traceable, readable, and macro-expandable:

[IR] LOAD R1, 5
[IR] LOAD R2, 3
[IR] ADD R3, R1
[IR] ADD R3, R2
[IR] FLOAT_LOAD F1, 3.14
[IR] FLOAT_ADD F1, F2
[IR] PRINT_FLOAT_SYSCALL F1


⸻

🛠 Macro System (C.I.A.M.S.)

❱ Inline Macro Format

|macro: fast_add| → expands into:
  LOAD R1, ...
  ADD R2, R1
  STORE ...

❱ .r4meta Bundles

{
  "fast_add": ["LOAD R1, $val1", "ADD R2, R1", "STORE result, R2"],
  "greet_user": ["PRINT $name"]
}

	•	🔁 Live .r4meta reload via filesystem watcher
	•	🔎 CLI + TUI inspection (--inspect-macros)
	•	⚙️ Export/share macro bundles (.zip with macro_bundle.json, icon.png, README.md)

⸻

🖥 TUI + CLI System

❱ CLI Flags

rexionc --file examples/hello_world.r4 --debug-full --meta --export-macros

❱ TUI Features
	•	Color-coded token stream, IR trace, macro overlay
	•	Real-time banner updates (⚡ Macro reloaded!)
	•	Toggle views: .r4, IR, .asm, Macro Expansion Tree
	•	Interactive macro drilldown (TAB to open trace chain)

⸻

🔄 Auto Compilation + Make

make            # builds compiler
make run        # compiles .r4 → .exe
make export     # zips macro bundles

Directories:
	•	/examples/ — .r4 test cases
	•	/build/ — compiled outputs
	•	/official/ — grammar, tmLanguage, codex docs
	•	/docs/ — Markdown & PDF output via generate_codex.py

⸻

📦 Macro Bundle System
	•	.r4meta file holds all macros
	•	macro_bundle.json manifests include:
	•	name, version, author
	•	dependencies, description
	•	icon.png, README.md
	•	✅ CLI autocompletion (--complete-macros)
	•	🔁 Dynamic macro reload (--reload-macros)
	•	📦 Macro exporter (--export-macros)

⸻

🧾 Documentation Generator

python3 generate_codex.py

	•	Extracts // DOC: lines from source
	•	Outputs:
	•	codex.md — Human-readable Markdown
	•	codex.pdf — Auto-styled printable document
	•	Supports cross-file navigation and macro documentation

⸻

🧪 Execution Features
	•	rexion.exe runs natively on x86_64
	•	Uses NASM + LD under the hood
	•	IR-to-ASM fully traceable
	•	Float and int output via:
	•	printf
	•	syscall + custom float_to_str / int_to_str
	•	Optional --benchmark mode

⸻

💬 Live Examples

define val: float;
val = 3.14;
print val;

|macro: clear_and_init|

rexionc --file=examples/init.r4 --meta --debug-full


⸻

🔮 Future Goals
	•	⏱ Lazy-loading macros per file
	•	🤖 AI-suggested macros based on patterns
	•	🎮 TUI replay mode for instruction tracing
	•	📊 Graph-based codex viewer (nodes for macro expansions)
	•	🧬 JIT inlining optimizer with predictive cache

⸻

Here is a professionally structured, contributor-ready README.md scaffold tailored for the Rexion Language & Compiler Ecosystem:

⸻


# ⚙️ REXION Language & Compiler Ecosystem

**Welcome to Rexion** — a revolutionary hybrid language and compiler system designed for performance, clarity, intrinsic self-linking rulesets, and intuitive low-to-high-level expressivity. Whether you're here to contribute, learn, optimize, or explore the core architecture of `.r4` → `.rexasm` → `.asm` → `.exe`, you're in the right place.

---

## 🌐 Ecosystem Highlights

- 📜 `.r4` Source Language – clean, semi-symbolic, instructional logic with macro-first semantics
- 🧠 C.I.A.M.S. (Contextually Inferred Abstracted Macro Scripts) – powerful inline macros for IR expansion
- ⚡ Macro Bundles – live-reloading `.r4meta` macro definitions, shareable and ZIP-exportable
- 🔧 AOT Compiler + JIT Runtime + Debug TUI
- 🔍 Full IR + ASM + Token Tracing Views
- 💬 CLI Tools & Interactive Dashboards
- 🛠️ Real NASM64 Assembly Emission & `.exe` Generation
- 📘 Auto Codex Generator: `.md` + `.pdf` from inline `// DOC:` lines

---

## 📁 Project Structure

rexion/
├── src/
│   ├── lexer.c / parser.c / ir_codegen.c / rexionc_main.c
│   ├── tui.c / watch_macros.c / codex_gen.c
│   └── headers/ (token_type.h, parser.h, etc.)
├── examples/
│   └── hello_world.r4
├── macros/
│   ├── macros.r4meta
│   └── macro_bundle.json
├── build/
│   └── *.rexasm / *.asm / .exe
├── official/
│   ├── Rexion.g4 / r4.tmLanguage.json
├── docs/
│   └── Order Of Operation.txt / Codex_.md / *.pdf
├── run_main.sh
├── Makefile
└── README.md

---

## 🚀 Getting Started

### 🔧 Requirements

- C Compiler (GCC/Clang)
- `nasm` + `ld` (for `.asm` → `.exe`)
- `make`
- Python 3 (for `generate_codex.py`, `.r4meta` tooling)
- Optional: `pandoc` for `.pdf` codex generation

### ✅ Build & Run

```bash
make         # builds the compiler
make run     # runs against examples/hello_world.r4
make tui     # launches the interactive visual interface
make export-macros  # zips macros.r4meta and macro_bundle.json


⸻

🧠 Language Overview

define score: int;
score = 98;

func greet() {
    print score;
}

class Student extends Person {
    private define age: int;
    public func speak() {
        print this.age;
    }
}

	•	|macro| definitions expand directly into IR blocks
	•	== denotes constant binding; = is mutable
	•	Supports: new, this, super, visibility (public, private, protected)

⸻

🔥 Debug, Trace, and Visualize

./rexionc --debug-full --meta macros/macros.r4meta examples/hello_world.r4

	•	Live .r4meta reload
	•	Macro pulse visualization (⚡ Macro Reloaded!)
	•	Interactive IR/ASM/Token stepping

⸻

🤝 Contributing
	1.	Fork + Clone
	2.	make dev to enter development mode (watchers + logging)
	3.	Use // DOC: tags to annotate for the codex
	4.	Submit PRs against the develop branch
	5.	Run generate_codex.py before final commits

⸻

📦 Macro Bundles

make export-macros
# outputs: macro_bundle.zip

Includes:
	•	macros.r4meta
	•	macro_bundle.json (metadata)
	•	Optional: icon.png, README.md

Supports import/export across projects and sharing in CI/CD pipelines.

⸻

💡 Inspirations

Rexion draws from:
	•	NASM64, LLVM IR, and handcrafted compilers
	•	Instruction-oriented programming theory
	•	Symbolic logic and macro-linked IR design
	•	Tactical control flow, minimal syntax, and full-stack generation

⸻

📣 Join the Project

We’re building something immense — compiler, debugger, assembler, macro system, and beyond.

If you’re passionate about:
	•	Programming language theory
	•	Compiler engineering
	•	Macro-based DSLs
	•	TUI/CLI/IR pipeline tools

📬 We want you on board.

⸻

🧾 License

Modified QSRLC 2.0  License © 2025 Violet Aura Creations Universe

---

Let me know if you'd like this tailored into a version for GitHub Pages or turned into an animated CLI onboarding script.

Here’s a fully structured and contributor-ready **`README.md`** scaffold for your Rexion public repository, tailored for professionalism, clarity, and engagement:

---

```markdown
# 🧠 Rexion Language

> A high-performance, macro-traceable, NASM-emitting instruction-oriented language with JIT + AOT, live IR/ASM, and a full interactive toolchain.

---

## 🔥 Overview

Rexion is a systems-level language built for speed, clarity, and extreme modularity. With native support for:
- ✅ **Static AOT + JIT** execution
- ✅ **NASM x64 + syscall output**
- ✅ **Macro rewriting from `.r4meta`**
- ✅ **Live IR + ASM debugging**
- ✅ **Flowchart-style AST representation**
- ✅ **TUI + CLI dual-layer interaction**
- ✅ **Token tracking, IR macros, bundled zip packs**
- ✅ **Full codex/doc generation from inline `// DOC:` blocks**

Rexion is a compiler project with ambitious design: a language *and* its own macro-driven ecosystem.

---

## 📦 Project Structure

| Path/File               | Purpose                                            |
|-------------------------|----------------------------------------------------|
| `/examples/`            | Example `.r4` source files                         |
| `/official/`            | Grammar, codex scrolls, `.tmLanguage`, etc.       |
| `/build/`               | Output binaries (`.rexasm`, `.asm`, `.exe`, etc.) |
| `/docs/`                | Generated Markdown and PDF docs                   |
| `lexer.c` / `parser.c`  | Core compiler frontend                            |
| `ir_codegen.c`          | Intermediate representation + NASM backend        |
| `rexionc_main.c`        | Top-level CLI and full pipeline executor          |
| `watch_macros.c`        | Background thread for `.r4meta` hot reload        |
| `generate_codex.py`     | Extracts `// DOC:` blocks → `codex.md` + `pdf`     |
| `.r4meta`               | Macro registry and metaprogramming definitions    |
| `Makefile`              | Full build + run toolchain                        |

---

## 🛠 Getting Started

### 🔧 Requirements

- `make`, `gcc`, `nasm`, `ld`
- `python3` (for doc generation)
- Optional: `xdotool`, `ncurses` (for advanced TUI)

### 🧪 Build & Run

```bash
make
make run FILE=examples/hello_world.r4

To watch macro changes and auto-refresh:

./rexionc --file=examples/hello_world.r4 --debug-full --meta


⸻

🎯 CLI Flags

Flag	Description
--file=	Source .r4 file to compile
--debug-full	Full token + IR + ASM debug pane
--meta	Load .r4meta macro bundle
--inspect-macros	TUI toggle to view macro expansions
--reload-macros	Force reload macros from disk
--export-macros	Bundle macros into distributable zip
--benchmark	Time performance of compilation + runtime


⸻

📚 Codex Documentation

To generate Markdown + PDF docs from inline source // DOC: comments:

python3 generate_codex.py

Outputs:
	•	/docs/codex.md
	•	/docs/codex.pdf

⸻

📦 Macro Bundles (.r4meta)

Macro bundles allow abstracted scripts to be reused, shared, and traced into .rexasm.

Export a bundle:

make export-macros

Inspect loaded macros in the TUI:

./rexionc --inspect-macros

Each bundle includes:
	•	macro_bundle.json
	•	.r4meta source
	•	README.md + optional icon.png

⸻

🖥 TUI Features
	•	Real-time token view, IR tree, ASM log
	•	Macro expansion trace + status (⚡ Macro Reloaded)
	•	Step-by-step execution trace mode (⏯️)
	•	Pulse banners + inline registers
	•	Autocompletion + definition jump

⸻

🤝 Contributing

Pull requests and contributions are encouraged. Here are areas you can help with:
	•	🧠 Optimizing compilation + JIT
	•	🧩 Expanding macro packs or .r4meta
	•	📖 Adding examples or tutorials
	•	🎨 Enhancing TUI styling and inspection tools
	•	🧪 Writing test suites or integration scripts

🔧 Local Dev Setup

git clone https://github.com/JOEYSOPRANO420/rexion
cd rexion
make


⸻

💡 Tips for Contributors
	•	Follow naming convention: CamelCase for functions, snake_case for variables
	•	Prefix macro trace logs with [TRACE]
	•	Keep docstrings short, but use // DOC: lines generously

⸻

📜 License

Modified QSRLC 2.0  License © 2025
Crafted with ⚙️ and 💜 by Violet Aura Creations and the Rexion Dev Guild

⸻

🌐 Links
	•	💬 Discord: [coming soon]
	•	📘 Documentation Site: [coming soon]
	•	📦 Macro Marketplace: [planned]

⸻


---


