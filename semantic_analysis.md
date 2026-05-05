# Semantic Analyzer Architecture (MiniC)

This file explains the architecture of the semantic analysis implementation in `semantic.c`.

## Position in Pipeline

Execution order in `main.c`

1. Parse source into AST (`yyparse`)
2. Run semantic pass (`semantic_analyze(ast_root)`)
3. If semantic pass succeeds, continue to IR generation

If semantic analysis reports errors, compilation stops.

## Module Boundaries

- `semantic.h`
  - Public entry point: `int semantic_analyze(ASTNode *root);`
- `semantic.c`
  - Internal type system
  - Symbol/scope management
  - Struct table management
  - AST traversal + rule checking

The semantic pass is read-only with respect to AST shape: it validates, reports errors, and returns success/failure.

## Core Data Structures

### 1. `Type`

Internal type model used only by semantic analysis:

- primitive: `void`, `bool`, `char`, `int`, `float`, string
- `struct`
- `array`
- `function`
- `invalid` (error-propagation type)

It also stores extra metadata:
- `base` for arrays
- `name` for struct types
- `ret` + `params` for function types

### 2. `Symbol` and `Scope`

- `Symbol`: `{ name, type, line }`
- `Scope`: linked stack frame with:
  - local symbol list
  - `parent` pointer

This supports:
- local lookup (`scope_lookup_local`)
- lexical lookup through parents (`scope_lookup`)
- shadowing across nested scopes

### 3. `StructDef`

Global linked list of known struct definitions:
- struct name
- member scope (field symbols)

Used by:
- `struct` type resolution
- member access validation (`obj.field`)

### 4. `SemCtx`

Traversal context:
- current scope
- struct registry
- current function return type
- loop depth counter
- error count

## Traversal Architecture

Semantic traversal is split into three layers:

1. `semantic_decl` for top-level and declaration-like nodes
2. `semantic_stmt` for statements/control-flow
3. `semantic_expr` for expression typing/checking

This separation keeps rules local and avoids mixing declaration and expression logic.

## Analyzer Flow

`semantic_analyze` does:

1. Create global scope
2. Register built-ins (`printf`, `scanf`) as variadic function symbols
3. Walk top-level AST declarations
4. Return non-zero if any semantic error was emitted

## Rule Groups Implemented

### Scope / symbol rules
- redeclaration in same scope is rejected
- undeclared identifier use is rejected
- duplicate parameter names rejected

### Type compatibility rules
- assignment/init compatibility (`can_assign`)
- operator constraints:
  - arithmetic: numeric
  - bitwise/shift/mod: integer-like
  - logical: scalar
  - comparison: numeric

### Function rules
- function symbol creation at declaration
- call target must be function type
- argument count/type checking
- return statement checked against current function return type

### Control-flow rules
- condition expressions in `if/while/do-while/for` must be scalar
- `break` and `continue` require `loop_depth > 0`

### Struct/array rules
- struct type must exist before use
- duplicate struct fields rejected
- member access validates field existence
- indexing requires array expression and integer-like index

## Error Reporting Strategy

All errors are emitted with source line context:

`[Semantic Error] Line <n>: <message>`

The pass does not stop on first error; it accumulates errors so one run gives multiple diagnostics.

## Known Simplifications

- Pointer semantics are intentionally minimal (`addr`/`deref` are not fully typed pointers yet).
- Type objects allocated during analysis are not globally interned/freed (acceptable for this project stage).
