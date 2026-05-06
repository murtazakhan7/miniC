# Semantic Analysis in MiniC

This compiler now runs semantic analysis after parsing and AST construction.

## Pipeline

The flow is now:

1. Lexer (`lexer.l`)
2. Parser + AST build (`parser.y`, `ast.c`)
3. Semantic analysis (`semantic.c`)
4. Success/failure report in `main.c`

If parsing succeeds but semantic checks fail, compilation exits with an error.

## Files Added/Updated

- Added `semantic.h`
- Added `semantic.c`
- Updated `main.c` to invoke `semantic_analyze(ast_root)`
- Updated `Makefile` to compile and link `semantic.c`

## What the Semantic Pass Checks

The semantic pass walks the AST and validates meaning, not syntax.

### 1. Symbol and Scope Rules

- Scoped symbol tables are used for globals, function scopes, compound blocks, and `for` init scopes.
- Redeclaration in the same scope is rejected.
- Use of undeclared identifiers is rejected.
- Duplicate function parameter names are rejected.

### 2. Type System Checks

Supported internal type kinds:

- `void`, `bool`, `char`, `int`, `float`, string-literal type
- `struct` types
- array types
- function types

Checks include:

- Assignment compatibility
- Variable initializer compatibility
- Array initializer element compatibility
- Arithmetic operators require numeric operands
- Bitwise/shift/mod operators require integer-like operands
- Logical operators require scalar operands
- Comparisons require numeric operands
- Ternary branch compatibility

### 3. Function Semantics

- Function names cannot be redeclared in the same scope.
- Function calls must target function-typed symbols.
- Function call argument count must match parameter count.
- Function call argument types are validated against parameter types.
- Return expressions are checked against the enclosing function return type.

### 4. Control-Flow Rules

- `if`, `while`, `do-while`, and `for` conditions must be scalar.
- `break` is valid only inside a loop.
- `continue` is valid only inside a loop.

### 5. Array and Struct Rules

- Array indexing requires integer-like index expressions.
- Indexing requires an array expression.
- Struct type usage is validated (`struct X` must be declared).
- Struct redeclaration is rejected.
- Duplicate struct fields are rejected.
- Member access validates that:
  - the left expression is a struct type
  - the field exists

## Error Format

Semantic errors are reported as:

`[Semantic Error] Line <line>: <message>`

Example categories:

- undeclared identifier
- redeclaration
- assignment type mismatch
- bad function call
- return type mismatch
- break/continue outside loop

## Limitations / Notes

- The pass is intentionally conservative and focused on the language features already present in your AST.
- Pointer-specific semantics (`addr`/`deref`) are currently treated minimally because pointer types are not fully modeled yet.
- Type objects are allocated as needed during analysis; this is acceptable for this compiler stage but can be optimized later.
