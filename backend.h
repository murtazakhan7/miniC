/*
 * backend.h — Backend interface and TAC contract for Member 3
 *
 * PURPOSE
 *   Defines the formal contract between IR generation (Member 2) and the
 *   backend optimization/codegen pipeline (Member 3).
 *   Provides structured in-memory TAC representation, optimization APIs,
 *   and register allocation interfaces.
 */

#ifndef BACKEND_H
#define BACKEND_H

#include <stdio.h>

/* =========================================================================
   TAC Instruction Types
   ========================================================================= */

typedef enum {
    /* Variable/assignment */
    TAC_VAR,              /* var name [, size] — declare a variable or array */
    TAC_ASSIGN,           /* dest = src */
    TAC_ASSIGN_CONST,     /* dest = constant (int/float/char/bool) */
    
    /* Binary operations */
    TAC_BINOP,            /* dest = left op right (arithmetic/logical/bitwise) */
    
    /* Unary operations */
    TAC_UNOP,             /* dest = op operand */
    
    /* Array operations */
    TAC_ARRAY_LOAD,       /* dest = array[index] */
    TAC_ARRAY_STORE,      /* array[index] = value */
    
    /* Struct member access */
    TAC_MEMBER_LOAD,      /* dest = obj.field or obj->field */
    TAC_MEMBER_STORE,     /* obj.field = value or obj->field = value */
    
    /* Function calls */
    TAC_PARAM,            /* param operand — pass argument */
    TAC_CALL,             /* dest = call func, argcount */
    
    /* Control flow */
    TAC_LABEL,            /* L123: — label marker */
    TAC_JUMP,             /* goto L123 */
    TAC_COND_JUMP,        /* if cond goto L123 */
    
    /* Return */
    TAC_RETURN,           /* return [value] */
    
    /* Markers for loop boundaries (may be eliminated in later passes) */
    TAC_BREAK,
    TAC_CONTINUE,
} TACOpKind;

/* =========================================================================
   Operand representation
   ========================================================================= */

typedef enum {
    OPERAND_INVALID,
    OPERAND_CONST_INT,       /* .int_val */
    OPERAND_CONST_FLOAT,     /* .float_val */
    OPERAND_CONST_BOOL,      /* .int_val (0 or 1) */
    OPERAND_CONST_CHAR,      /* .int_val (as char) */
    OPERAND_TEMP,            /* .name (e.g., "t0") */
    OPERAND_VAR,             /* .name (source variable) */
    OPERAND_LABEL,           /* .name (e.g., "L0") */
    OPERAND_STRING,          /* .str_val (string literal) */
} OperandKind;

typedef struct {
    OperandKind kind;
    union {
        int int_val;        /* for CONST_INT, CONST_BOOL, CONST_CHAR */
        double float_val;   /* for CONST_FLOAT */
        char *name;         /* for TEMP, VAR, LABEL */
        char *str_val;      /* for STRING */
    };
} Operand;

/* =========================================================================
   TAC Instruction
   ========================================================================= */

typedef struct {
    TACOpKind op;
    int line;                    /* source line for error reporting */
    
    Operand dest;                /* destination (assignment target) */
    Operand left;                /* first source operand */
    Operand right;               /* second source operand */
    char *attr;                  /* for BINOP: operator; for CALL: function name; etc */
    int attr_int;                /* auxiliary integer (e.g., array size, arg count) */
} TACInstr;

/* =========================================================================
   TAC Function (collection of instructions)
   ========================================================================= */

typedef struct {
    char *name;
    int param_count;
    TACInstr **instrs;
    int instr_count;
    int instr_capacity;
} TACFunc;

/* =========================================================================
   TAC Program (collection of functions + globals)
   ========================================================================= */

typedef struct {
    TACFunc **funcs;
    int func_count;
    int func_capacity;
    
    TACInstr **globals;          /* global variable declarations */
    int global_count;
    int global_capacity;
} TACProgram;

/* =========================================================================
   Backend API
   ========================================================================= */

/* Create empty TAC program */
TACProgram *tac_program_new(void);

/* Free TAC program and all contents */
void tac_program_free(TACProgram *prog);

/* Create and append a function */
TACFunc *tac_program_add_func(TACProgram *prog, const char *name, int param_count);

/* Append an instruction to a function */
void tac_func_add_instr(TACFunc *func, TACInstr instr);

/* Append a global variable declaration */
void tac_program_add_global(TACProgram *prog, TACInstr instr);

/* Print TAC program to file (for debugging) */
void tac_print(TACProgram *prog, FILE *out);

/* =========================================================================
   Optimization Pass Interface
   ========================================================================= */

typedef enum {
    OPTPASS_CONST_FOLD,
    OPTPASS_DEAD_CODE,
    OPTPASS_CSE,
} OptPassKind;

/* Run all optimization passes in phase-weighted order */
void tac_optimize(TACProgram *prog);

/* Run a single optimization pass */
void tac_optimize_pass(TACProgram *prog, OptPassKind pass);

/* =========================================================================
   Codegen Interface
   ========================================================================= */

/* Generate x86 assembly from optimized TAC */
void tac_codegen_x86(TACProgram *prog, FILE *out);

#endif /* BACKEND_H */
