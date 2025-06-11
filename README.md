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
| **Indentation**     | Optional (structured via flow brackets or `->`)                                |
| **Execution**       | AOT (default) + optional JIT (hybrid runtime optimization)                     |
| **Output**          | Raw machine code + IR mapping table                                            |
| **Optimization**    | On-the-fly during JIT, static during AOT                                       |
| **Error Handling**  | Live, context-aware, non-fatal diagnostic pipelines                            |
| **Memory Handling** | Deep, manual + abstracted intrinsics, no GC unless invoked explicitly          |
| **IR Structure**    | Serialized table: `Instruction Flow → Context Reference → Abstraction Mapping` |
| **AST**             | Flow-chart model: `Instruction (NASM-style)` → `Hex` Mapping                   |

---

### 🧾 **{Example Syntax}**

**(Rexion)**

func greet(name)
{
  print "Hello, " + name
}

greet("Violet")


Can also be written with NASM-style explicitness:

**(Rexion)**

@func greet(name) -> {
  ::print "Hello, " + name
}

@call greet("Violet")


Or minimal inline NASM-inspired version:

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
3. **AST Builder** — Converts code into `Flow-NASM-Hex` representation
4. **IR Serializer** — Stores all instructions in reference-linked mapping table
5. **AOT Compiler** — Converts IR to optimized machine code
6. **JIT Layer (optional)** — Optimizes branches in real time based on live metrics
7. **Execution Engine** — Runs machine code or hybrid code block

---

### 🧠 **[Memory Architecture]**

* Manual allocation (`alloc`, `free`) for low-level dev
* Auto-scope-based cleanup optional (`autofree`)
* Explicit memory zones: `zone stack`, `zone heap`, `zone volatile`
* Intrinsic commands like:

  **(Rexion)**
  
  zone(heap): alloc 256b as buffer
  zone(stack): write buffer "Data"


---

### 📌 **Minimal Punctuation Rules**

* `::` for instruction block
* `->` for directional flow / return
* `@` for declarations
* `>> <<` for instruction groupings
* `#` for comment

---


