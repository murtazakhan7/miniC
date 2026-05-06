# IR Generation Architecture (MiniC)

This file explains the architecture of the intermediate code generator in `ir.c`.

## Position in Pipeline

IR generation runs only after semantic success:

1. Parse + AST build
2. Semantic analysis
3. TAC generation (`ir_generate(ast_root, stdout)`)

So IR code assumes semantically valid AST input.

## Module Boundaries

- `ir.h`
  - Public API: `void ir_generate(ASTNode *root, FILE *out);`
- `ir.c`
  - TAC emission logic
  - expression lowering
  - statement lowering
  - function lowering

## IR Style

The generated IR is textual three-address code (TAC) with:

- temporaries: `t0`, `t1`, ...
- labels: `L0`, `L1`, ...
- explicit `param` before `call`
- function delimiters: `func name:` and `endfunc name`

Examples:
- `t3 = a + b`
- `if t3 goto L0`
- `goto L1`
- `param x`
- `t8 = call foo, 2`

## Core Data Structure

`IRGen` context carries:

- output stream (`FILE *out`)
- temp counter (`temp_id`)
- label counter (`label_id`)

Helpers:
- `new_temp()` -> unique temp names
- `new_label()` -> unique labels
- `emit()` -> one formatted TAC line

## Lowering Architecture

Lowering is split by AST category:

1. `gen_expr`:
   - returns a string representing computed value location (literal, variable, temp)
   - emits instructions for sub-computations
2. `gen_stmt`:
   - emits control-flow and side-effect statements
3. `gen_function`:
   - emits function prologue (`func`)
   - emits parameter definitions
   - lowers body
   - emits `endfunc`

`ir_generate` is the top-level dispatcher over program declarations.

## Expression Lowering Strategy

- literals / identifiers: returned directly as string operands
- binary operators: compute lhs/rhs, emit one TAC op into new temp
- assignments/compound assignments:
  - resolve lvalue (`ident`, `index`, `member`)
  - emit write to lvalue
  - return assigned value in temp
- unary inc/dec:
  - emits read-modify-write form
  - handles pre/post return behavior
- calls:
  - emit `param` per argument
  - emit `call` with arg count
  - capture return in temp
- ternary:
  - lowered with branch labels + merge temp

## Statement / Control-Flow Lowering

- `if/else`: condition branch + then/else/end labels
- `while`: head/body/end loop labels
- `do-while`: body-first then condition back-edge
- `for`: init, condition test, body, step, and back-edge labels
- `return`: emits `return` with or without value
- decls: emits `var` and optional init assignment
- array init-list: emits element-wise stores (`arr[i] = v`)

## Lvalue Representation

`lvalue_to_str` converts assignable AST nodes into text targets:

- `x`
- `arr[i]`
- `obj.field` / `obj->field`

This keeps assignment lowering centralized and consistent.

## Current Limitations

- `break`/`continue` are currently emitted as textual markers, not yet fully patched to concrete jump labels.
- Memory for generated operand strings is not reclaimed (acceptable for short-lived compiler process in this stage).
- IR is intentionally simple and linear for readability and next-stage experimentation.
