# MiniC Compiler 
## Front-End: Lexical Analyzer + Syntax Analyzer

**Compiler Name:** MiniC  
**Target Language:** Subset of C  
**Tools:** Flex (Lexer), Bison (Parser), C (Implementation)  


---

## 1. Project Structure

```
minic/
├── .gitignore                  ← Ignores build/, editor junk, local outputs
├── Makefile                    ← Build system (`BUILD_DIR=build` by default)
├── package.json                ← Optional Node tooling (not required to build MiniC)
├── package-lock.json
│
├── build/                      ← Created by `make` (gitignored)
│   ├── minic                   ← Compiler executable
│   ├── *.o                     ← Object files
│   ├── parser.tab.c / parser.tab.h   ← Bison from parser.y
│   ├── parser.output           ← Bison grammar report (with `bison -v`)
│   └── lex.yy.c                ← Flex from lexer.l
│
├── docs/                       ← All project Markdown except this README
│   ├── ARCHITECTURE.md         ← Frontend / middle-end / backend reference
│   ├── BACKEND.md              ← Backend design (Member 3)
│   ├── MEMBER1_DOCS.md         ← Lexer/parser notes (Member 1)
│   ├── ir_generation.md        ← IR generation notes
│   ├── semantic_analysis.md    ← Semantic analysis notes
│   └── SEMANTIC_ANALYSIS.md    ← Additional semantic notes
│
├── lexer.l                     ← Flex lexical analyzer (Member 1)
├── parser.y                    ← Bison grammar + AST (Member 1)
├── ast.h / ast.c               ← AST definitions (Member 1)
├── semantic.h / semantic.c     ← Semantic analysis (Member 2)
├── ir.h / ir.c                 ← TAC / IR generation (Member 2)
├── backend.h / backend.c       ← Structured TAC IR (Member 3)
├── optimizer.h / optimizer.c   ← Optimization passes (Member 3)
├── regalloc.h / regalloc.c     ← Register allocation (Member 3)
├── codegen.c                   ← x86-64 codegen (Member 3)
├── main.c                      ← Compiler driver (CLI)
│
├── README.md                   ← This file (root overview; all other docs live in docs/)
│
└── tests/
    ├── test1.c                 ← Basic: variables, functions, if/else, printf
    ├── test2.c                 ← Loops, structs, arrays, bitwise ops
    ├── test3.c                 ← Structs, casting, hex/octal literals, member access
    └── test_opt.c              ← Optimization test cases (Member 3)
```

**Generated build artifacts** (under `build/`; do not edit—regenerate with `make`):

| Artifact | Produced by |
|----------|-------------|
| `build/parser.tab.c`, `build/parser.tab.h` | Bison from `parser.y` |
| `build/parser.output` | Bison `-v` (state/grammar report) |
| `build/lex.yy.c` | Flex from `lexer.l` |
| `build/*.o`, `build/minic` | Compiler and link step |

Compiler output files (e.g. `out.s`, `out.s.log`) are typically written where you pass `-o`; by default these often land in the project root (see `.gitignore`).

---

## 2. Build Instructions

### Prerequisites
```bash
sudo apt install flex bison gcc make
```

### Build
```bash
make          # builds ./build/minic (and everything under build/)
make clean    # deletes the entire build/ directory
```

### Run
```bash
./build/minic --tokens source.c    # print token stream (lexer only mode)
./build/minic --ast source.c       # parse and print full AST
./build/minic source.c             # semantic + TAC build + TAC optimize + print final TAC
./build/minic --emit-ir source.c   # also print pre-optimization IR text
./build/minic --no-opt source.c    # skip TAC optimization
./build/minic --codegen source.c   # additionally print x86-64 assembly
./build/minic --codegen -o out.s source.c  # write pipeline output to out.s (overwrites if exists)
```

### Run tests
```bash
make test      # runs all tests/*.c through the compiler
make tokens    # shows token output for tests/test1.c
make ast       # shows AST output for tests/test1.c
```

---

## 3. Language Features Supported

### 3.1 Data Types
| Type | MiniC Syntax | Notes |
|------|-------------|-------|
| Integer | `int` | 32-bit signed |
| Float | `float` | Double-precision internally |
| Character | `char` | Single char with `'x'` literal |
| Boolean | `bool` | `true` / `false` literals |
| Void | `void` | Return type only |
| Struct | `struct Name { ... };` | User-defined aggregate |

### 3.2 Literals Recognized by Lexer
| Category | Examples | Token |
|----------|---------|-------|
| Integer | `42`, `0xFF`, `0777` | `INT_LITERAL` |
| Float | `3.14`, `2.0e-3` | `FLOAT_LITERAL` |
| Character | `'A'`, `'\n'` | `CHAR_LITERAL` |
| String | `"hello %d"` | `STR_LITERAL` |
| Boolean | `true`, `false` | `BOOL_LITERAL` |

### 3.3 Operators (all recognized)
| Category | Operators |
|----------|-----------|
| Arithmetic | `+` `-` `*` `/` `%` |
| Relational | `==` `!=` `<` `>` `<=` `>=` |
| Logical | `&&` `\|\|` `!` |
| Assignment | `=` `+=` `-=` `*=` `/=` `%=` `&=` `\|=` `^=` `<<=` `>>=` |
| Bitwise | `&` `\|` `^` `~` `<<` `>>` |
| Increment | `++` `--` (prefix and postfix) |
| Member | `.` `->` |
| Ternary | `? :` |

### 3.4 Statements
- Variable declarations (with or without initializer, including arrays)
- Function definitions (with parameter lists)
- `if` / `if-else` (dangling-else resolved by Bison, 1 known S/R conflict, expected)
- `while` loop
- `do-while` loop
- `for` loop (init/cond/incr all optional)
- `return` (with or without expression)
- `break` / `continue`
- Compound statements `{ ... }`
- Empty statement `;`
- Expression statement `expr;`

### 3.5 Expressions (full precedence tower)
```
Lowest  → assign: =  +=  -=  ...  (right-associative)
        → ternary: ?:
        → logical-or: ||
        → logical-and: &&
        → bitwise-or: |
        → bitwise-xor: ^
        → bitwise-and: &
        → equality: ==  !=
        → relational: <  >  <=  >=
        → shift: <<  >>
        → additive: +  -
        → multiplicative: *  /  %
        → unary: neg  !  ~  pre++  pre--  &(addr)  *(deref)  (type)cast
Highest → postfix: []  ()  .  ->  post++  post--
```

---

## 4. Lexer — Sample Output (test1.c)

**Command:** `./build/minic --tokens tests/test1.c`

```
=== MiniC Compiler ===
Source: tests/test1.c

LINE  TOKEN                LEXEME
----  -----                ------
3     KW_INT               int
3     IDENTIFIER           x
3     OP_ASSIGN            =
3     INT_LITERAL          10
3     SEMICOLON            ;
4     KW_FLOAT             float
4     IDENTIFIER           pi
4     OP_ASSIGN            =
4     FLOAT_LITERAL        3.14
4     SEMICOLON            ;
5     KW_CHAR              char
5     IDENTIFIER           grade
5     OP_ASSIGN            =
5     CHAR_LITERAL         'A'
5     SEMICOLON            ;
6     KW_BOOL              bool
6     IDENTIFIER           flag
6     OP_ASSIGN            =
6     BOOL_LITERAL         true
6     SEMICOLON            ;
8     KW_INT               int
8     IDENTIFIER           add
8     LPAREN               (
8     KW_INT               int
8     IDENTIFIER           a
8     COMMA                ,
8     KW_INT               int
8     IDENTIFIER           b
8     RPAREN               )
8     LBRACE               {
9     KW_RETURN            return
9     IDENTIFIER           a
9     OP_PLUS              +
9     IDENTIFIER           b
9     SEMICOLON            ;
10    RBRACE               }
12    KW_INT               int
12    IDENTIFIER           main
...
```

### Token Categories (Complete List)

**Keywords (15):**
`KW_INT` `KW_FLOAT` `KW_CHAR` `KW_BOOL` `KW_VOID`  
`KW_IF` `KW_ELSE` `KW_WHILE` `KW_DO` `KW_FOR` `KW_RETURN`  
`KW_STRUCT` `KW_BREAK` `KW_CONTINUE`  
`KW_PRINTF` `KW_SCANF`

**Literals (5):**
`INT_LITERAL` `FLOAT_LITERAL` `CHAR_LITERAL` `STR_LITERAL` `BOOL_LITERAL`

**Identifiers (1):**
`IDENTIFIER`

**Operators (30+):**
`OP_PLUS` `OP_MINUS` `OP_MUL` `OP_DIV` `OP_MOD`  
`OP_ASSIGN` `OP_PLUS_ASSIGN` `OP_MINUS_ASSIGN` ... (all compound assignments)  
`OP_EQ` `OP_NEQ` `OP_LT` `OP_GT` `OP_LEQ` `OP_GEQ`  
`OP_AND` `OP_OR` `OP_NOT`  
`OP_INC` `OP_DEC`  
`OP_BITAND` `OP_BITOR` `OP_BITXOR` `OP_BITNOT` `OP_LSHIFT` `OP_RSHIFT`  
`OP_ARROW` `OP_DOT`

**Punctuation (9):**
`LPAREN` `RPAREN` `LBRACE` `RBRACE` `LBRACKET` `RBRACKET`  
`SEMICOLON` `COMMA` `COLON` `QUESTION`

**Skipped (not tokenized):**
- Single-line comments `// ...`
- Multi-line comments `/* ... */`
- Preprocessor directives `#include`, `#define`
- Whitespace and newlines

---

## 5. Parser — Sample AST Output (test1.c)

**Command:** `./build/minic --ast tests/test1.c`

```
=== MiniC Compiler ===
Source: tests/test1.c

=== Abstract Syntax Tree ===
Program  (6 top-level declarations)
  VarDecl [line 3]: x
    Type: Type(int)
    Init:
      IntLit(10)
  VarDecl [line 4]: pi
    Type: Type(float)
    Init:
      FloatLit(3.14)
  VarDecl [line 5]: grade
    Type: Type(char)
    Init:
      CharLit('A')
  VarDecl [line 6]: flag
    Type: Type(bool)
    Init:
      BoolLit(true)
  FuncDef [line 10]: add
    RetType: Type(int)
    Params (2):
      Param: a — Type(int)
      Param: b — Type(int)
    Body:
      Compound (1 stmts)
        Return
          BinOp '+'
            Ident(a)
            Ident(b)
  FuncDef [line 24]: main
    RetType: Type(int)
    Params (0):
    Body:
      Compound (5 stmts)
        VarDecl [line 13]: result
          Type: Type(int)
          Init:
            Call (2 args)
              Callee:
                Ident(add)
              Args:
                Ident(x)
                IntLit(5)
        VarDecl [line 14]: area
          Type: Type(float)
          Init:
            BinOp '*'
              Ident(pi)
              FloatLit(2)
        If
          Cond:
            BinOp '>'
              Ident(result)
              IntLit(10)
          Then:
            Compound (1 stmts)
              ExprStmt
                BinOp '='
                  Ident(result)
                  BinOp '-'
                    Ident(result)
                    IntLit(1)
          Else:
            Compound (1 stmts)
              ExprStmt
                BinOp '='
                  Ident(result)
                  BinOp '+'
                    Ident(result)
                    IntLit(1)
        ExprStmt
          Call (2 args)
            Callee:
              Ident(printf)
            Args:
              StrLit("%d")
              Ident(result)
        Return
          IntLit(0)

Parsing successful.
```

---

## 6. AST Node Reference (for Members 2 & 3)

All AST types are defined in `ast.h`. Every node has:
- `NodeKind kind` — the type discriminator
- `int line` — 1-based source line number

### Node Kinds

```c
// Top level
NODE_PROGRAM        // .program.decls  → NodeList* of top-level declarations

// Declarations
NODE_FUNC_DEF       // .func_def.ret_type, .name, .params (NodeList*), .body
NODE_VAR_DECL       // .var_decl.type, .name, .init (may be NULL), .arr_size (0 = not array)
NODE_ARRAY_DECL     // .arr_decl.type, .name, .size, .init_list (NodeList*)
NODE_STRUCT_DECL    // .struct_decl.name, .members (NodeList* of NODE_VAR_DECL)
NODE_PARAM          // .param.type, .name, .is_array (1 if declared as type name[])

// Types
NODE_TYPE           // .type.name  → "int" | "float" | "char" | "bool" | "void"
NODE_STRUCT_TYPE    // .struct_type.name  → struct tag name

// Statements
NODE_COMPOUND       // .compound.stmts → NodeList*
NODE_IF             // .if_stmt.cond, .then_br, .else_br (NULL if no else)
NODE_WHILE          // .while_stmt.cond, .body
NODE_DO_WHILE       // .do_while_stmt.body, .cond
NODE_FOR            // .for_stmt.init, .cond, .incr (any may be NULL), .body
NODE_RETURN         // .ret_stmt.value (NULL for bare return)
NODE_BREAK          // no fields
NODE_CONTINUE       // no fields
NODE_EMPTY_STMT     // no fields
NODE_EXPR_STMT      // .expr_stmt.expr

// Expressions
NODE_BINOP          // .binop.op (string: "+","<=","&&","=","+=", etc.), .left, .right
NODE_UNOP           // .unop.op ("neg","!","~","pre++","pre--","post++","post--","addr","deref"), .operand
NODE_TERNARY        // .ternary.cond, .then_expr, .else_expr
NODE_CAST           // .cast.type (NODE_TYPE), .cast.expr
NODE_CALL           // .call.callee (usually NODE_IDENT), .args (NodeList*)
NODE_INDEX          // .index.array, .index.index
NODE_MEMBER         // .member.object, .field (char*), .arrow (1="->" / 0=".")
NODE_SIZEOF         // .sizeof_expr.type_or_expr, .is_type (1=sizeof(type) / 0=sizeof(expr))

// Literals
NODE_INT_LIT        // .int_lit.ival
NODE_FLOAT_LIT      // .float_lit.fval
NODE_CHAR_LIT       // .char_lit.cval
NODE_STR_LIT        // .str_lit.sval
NODE_BOOL_LIT       // .bool_lit.bval  (1=true / 0=false)

// Identifier
NODE_IDENT          // .ident.name
```

### NodeList API

```c
NodeList *nl_new(void);                       // empty list
NodeList *nl_single(ASTNode *node);           // list with one element
NodeList *nl_append(NodeList *list, ASTNode *node);  // append and return list

// Access:
list->count           // number of items
list->items[i]        // i-th ASTNode*
```

### Utility Functions

```c
void ast_print(ASTNode *node, int indent);   // pretty-print to stdout
void ast_free(ASTNode *node);               // recursively free entire tree
```

### Exported Global

```c
extern ASTNode *ast_root;   // set by parser after successful parse
```

---

## 7. Grammar Summary (BNF)

```
program         → translation_unit

translation_unit → ε
                 | translation_unit declaration

declaration     → function_def | var_decl | struct_decl

type_spec       → int | float | char | bool | void | struct IDENTIFIER

struct_decl     → struct IDENTIFIER { declaration_list } ;
var_decl        → type_spec IDENTIFIER ;
                | type_spec IDENTIFIER = expr ;
                | type_spec IDENTIFIER [ INT_LITERAL ] ;
                | type_spec IDENTIFIER [ INT_LITERAL ] = { arg_list } ;

function_def    → type_spec IDENTIFIER ( param_list ) compound_stmt
param           → type_spec IDENTIFIER
                | type_spec IDENTIFIER []

compound_stmt   → { stmt_list }
stmt            → var_decl | if_stmt | while_stmt | do_while_stmt
                | for_stmt | return_stmt | break_stmt | continue_stmt
                | compound_stmt | expr_stmt | ;

if_stmt         → if ( expr ) stmt
                | if ( expr ) stmt else stmt
while_stmt      → while ( expr ) stmt
do_while_stmt   → do stmt while ( expr ) ;
for_stmt        → for ( for_init ; for_cond ; for_incr ) stmt
return_stmt     → return expr ; | return ;

expr            → assign_expr
assign_expr     → ternary_expr
                | unary_expr (= | += | -= | ...) assign_expr
ternary_expr    → or_expr | or_expr ? expr : ternary_expr
...             (standard C precedence tower down to postfix)
unary_expr      → postfix_expr
                | -unary | !unary | ~unary | ++unary | --unary
                | &unary | *unary | (type_spec) unary
postfix_expr    → primary_expr
                | postfix [ expr ]
                | postfix ( arg_list )
                | postfix . IDENTIFIER
                | postfix -> IDENTIFIER
                | postfix++ | postfix--
primary_expr    → INT_LITERAL | FLOAT_LITERAL | CHAR_LITERAL
                | STR_LITERAL | BOOL_LITERAL | IDENTIFIER
                | printf | scanf
                | ( expr )
```

**Note:** There is 1 declared S/R conflict (`%expect 1`) for the classic dangling-else ambiguity, resolved by Bison's default (associate `else` with the nearest `if`). This is correct C behavior.

---

## 8. Error Reporting

### Lexer errors
```
[Lexer Error] Line <N>, Col <C>: Unknown character: '<char>'
```

### Parser errors
```
[Parse Error] Line <N>, Col <C>: syntax error
```

Both `line_num` and `col_num` are maintained by the lexer and available to `yyerror()`.

---

## 9. Handoff Notes for Member 2 (Semantic Analyzer + TAC)

### What you receive
- `ast_root` — a fully built AST after `yyparse()` returns 0
- `ast.h` — complete type definitions; include this in your semantic files
- `ast.c` — already compiled into `ast.o`; link it

### How to start your pass

```c
// In your semantic.c:
#include "ast.h"

extern ASTNode *ast_root;

void analyze(ASTNode *node) {
    if (!node) return;
    switch (node->kind) {
        case NODE_PROGRAM:
            for (int i = 0; i < node->program.decls->count; i++)
                analyze(node->program.decls->items[i]);
            break;
        case NODE_FUNC_DEF:
            // push scope, enter params, analyze body
            break;
        case NODE_VAR_DECL:
            // insert into symbol table, type-check init
            break;
        case NODE_BINOP:
            // type-check left and right, resolve result type
            break;
        // ... etc
    }
}
```

### Symbol Table hints
- MiniC has **block scope** — a stack of hash maps works well
- `NODE_FUNC_DEF` pushes a new scope; `NODE_COMPOUND` also pushes a scope
- `NODE_PARAM` entries go into the function's scope
- `NODE_STRUCT_DECL` — maintain a separate struct-type table keyed on name
- `NODE_IDENT` lookups climb the scope stack

### TAC generation hints
- Binary operators map directly: `t1 = left op right`
- `NODE_CALL` → evaluate args into temporaries, emit `param t_i`, then `call f, n`
- `NODE_IF` → evaluate cond, `ifFalse cond goto L_else`, emit then-branch, `goto L_end`, emit else
- `NODE_FOR` → `L_cond:`, test, `ifFalse goto L_end`, body, incr, `goto L_cond`, `L_end:`
- Array index `a[i]` → `t = i * sizeof(type)`, `t2 = a + t`, then load/store through `t2`

### Key invariants to check
1. Variables must be declared before use
2. Function call argument count must match parameter count
3. Assignment type compatibility (int←float needs implicit cast warning)
4. `return` expression type must match function return type
5. `break`/`continue` only valid inside a loop
6. Struct member access: left side of `.` must be struct type; `->` must be pointer-to-struct

---

## 10. Handoff Notes for Member 3 (Optimization + Machine Code)

You will receive **Three-Address Code (TAC)** from Member 2. The front-end has no TAC yet, but the AST structure that Member 2 will walk is fully documented in Section 6 above.

### Expected TAC format (to negotiate with Member 2)
```
t0 = 10
t1 = t0 + 5
if t1 > 10 goto L1
t1 = t1 + 1
goto L2
L1:
t1 = t1 - 1
L2:
```

### Optimization targets (from AST analysis)
1. **Constant folding** — `IntLit + IntLit` in a `NODE_BINOP` with literal children can be folded at TAC gen time
2. **Dead code** — `NODE_IF` where cond is a `NODE_BOOL_LIT` / `NODE_INT_LIT`
3. **CSE** — repeated `NODE_BINOP` subtrees with identical operands

---

## 11. Known Limitations (scope for extension)

| Feature | Status | Notes |
|---------|--------|-------|
| Pointer types `int *p` | Not supported | Unary `*` and `&` operators exist; type system has no pointer type node |
| `typedef` | Not supported | — |
| `#include` / `#define` | Skipped | Preprocessor lines skipped by lexer |
| `sizeof` | AST node exists | Not wired to grammar production yet |
| Multi-dim arrays | Not supported | Only 1D arrays |
| String operations | Not supported | `STR_LITERAL` tokenized, no stdlib |
| `scanf` format parsing | Not supported | `scanf` recognized as a call |
| Floating point `for` | Works | `float` loop vars parsed correctly |

---

## 12. Complete Compiler Pipeline

### Overall Flow

```
Source Code (.c)
    ↓ [Lexer - Member 1]
Tokens
    ↓ [Parser - Member 1]
Abstract Syntax Tree (AST)
    ↓ [Semantic Analyzer - Member 2]
Type-Checked AST
    ↓ [IR Generator - Member 2]
Three-Address Code (TAC)
    ↓ [Optimizer - Member 3]
Optimized TAC
    ↓ [Register Allocator - Member 3]
TAC with Register Assignments
    ↓ [Code Generator - Member 3]
x86-64 Assembly
```

### Member Responsibilities

**Member 1 — Lexer & Parser:**
- Flex lexical analyzer (`lexer.l`)
- Bison parser with AST construction (`parser.y`)
- AST node types and utilities (`ast.h`, `ast.c`)

**Member 2 — Semantic Analysis & TAC:**
- Semantic analysis pass with type checking and scoping (`semantic.h`, `semantic.c`)
- IR generation to Three-Address Code (`ir.h`, `ir.c`)

**Member 3 — Optimization & Code Generation:**
- Structured TAC IR definition (`backend.h`, `backend.c`)
- Optimization passes: constant folding, dead code elimination, CSE (`optimizer.h`, `optimizer.c`)
- Register allocation: liveness analysis, linear-scan (`regalloc.h`, `regalloc.c`)
- x86-64 code generation (`codegen.c`)
- Comprehensive documentation (`docs/BACKEND.md`)

### Usage

```bash
./build/minic source.c                          # Build IR/TAC, optimize TAC, print final TAC
./build/minic --tokens source.c                 # Lexer only
./build/minic --ast source.c                    # Parse and show AST
./build/minic --emit-ir --emit-tac source.c     # Print both IR text and final TAC
./build/minic --no-opt --emit-tac source.c      # Print unoptimized TAC
./build/minic --codegen source.c                # Run backend code generation (x86-64)
./build/minic --codegen -o out.s source.c       # Write output file (overwrite if exists)
make test                       # Run test suite
```

Pipeline control flags:
- `--no-ir` disables IR/TAC/backend pipeline stages
- `--no-tac` disables TAC optimization/printing/codegen
- `--no-opt` keeps TAC unoptimized
- `--emit-ir` prints IR textual stream produced by `ir_generate`
- `--emit-tac` / `--no-emit-tac` enable/disable final TAC print
- `--codegen` runs `tac_codegen_x86(...)`
- `-o <file>` or `--output <file>` writes pipeline output to file (truncate/overwrite mode)

---

## 13. Backend Architecture (Member 3)

For detailed information on the optimization and code generation backend, see **[`docs/BACKEND.md`](docs/BACKEND.md)**.
