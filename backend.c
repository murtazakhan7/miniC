/*
 * backend.c — Backend implementation (TAC IR, optimizations, codegen)
 *
 * Implements:
 * - Structured TAC IR construction and management
 * - Operand creation utilities
 * - TAC program allocation/deallocation
 * - Placeholder stubs for optimization and codegen (to be filled in by phases)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backend.h"
#include "optimizer.h"

/* =========================================================================
   Operand Construction Helpers
   ========================================================================= */

static Operand operand_invalid(void) {
    return (Operand){OPERAND_INVALID, {0}};
}

static Operand operand_const_int(int val) {
    return (Operand){OPERAND_CONST_INT, {.int_val = val}};
}

static Operand operand_const_float(double val) {
    return (Operand){OPERAND_CONST_FLOAT, {.float_val = val}};
}

static Operand operand_const_bool(int val) {
    return (Operand){OPERAND_CONST_BOOL, {.int_val = val ? 1 : 0}};
}

static Operand operand_const_char(char val) {
    return (Operand){OPERAND_CONST_CHAR, {.int_val = (int)val}};
}

static Operand operand_temp(const char *name) {
    Operand o = {OPERAND_TEMP, {0}};
    o.name = (char *)name;
    return o;
}

static Operand operand_var(const char *name) {
    Operand o = {OPERAND_VAR, {0}};
    o.name = (char *)name;
    return o;
}

static Operand operand_label(const char *name) {
    Operand o = {OPERAND_LABEL, {0}};
    o.name = (char *)name;
    return o;
}

static Operand operand_string(const char *val) {
    Operand o = {OPERAND_STRING, {0}};
    o.str_val = (char *)val;
    return o;
}

/* =========================================================================
   TAC Program Management
   ========================================================================= */

TACProgram *tac_program_new(void) {
    TACProgram *prog = calloc(1, sizeof(TACProgram));
    if (!prog) {
        perror("calloc(TACProgram)");
        exit(1);
    }
    prog->func_capacity = 16;
    prog->funcs = calloc(prog->func_capacity, sizeof(TACFunc *));
    if (!prog->funcs) {
        perror("calloc(funcs)");
        exit(1);
    }
    prog->global_capacity = 16;
    prog->globals = calloc(prog->global_capacity, sizeof(TACInstr *));
    if (!prog->globals) {
        perror("calloc(globals)");
        exit(1);
    }
    return prog;
}

void tac_program_free(TACProgram *prog) {
    if (!prog) return;
    
    for (int i = 0; i < prog->func_count; i++) {
        TACFunc *func = prog->funcs[i];
        if (func) {
            free(func->name);
            if (func->instrs) {
                for (int j = 0; j < func->instr_count; j++) {
                    if (func->instrs[j]) free(func->instrs[j]);
                }
                free(func->instrs);
            }
            free(func);
        }
    }
    free(prog->funcs);
    
    if (prog->globals) {
        for (int i = 0; i < prog->global_count; i++) {
            if (prog->globals[i]) free(prog->globals[i]);
        }
        free(prog->globals);
    }
    
    free(prog);
}

TACFunc *tac_program_add_func(TACProgram *prog, const char *name, int param_count) {
    if (prog->func_count >= prog->func_capacity) {
        prog->func_capacity *= 2;
        prog->funcs = realloc(prog->funcs, prog->func_capacity * sizeof(TACFunc *));
        if (!prog->funcs) {
            perror("realloc(funcs)");
            exit(1);
        }
    }
    
    TACFunc *func = calloc(1, sizeof(TACFunc));
    if (!func) {
        perror("calloc(TACFunc)");
        exit(1);
    }
    func->name = strdup(name);
    func->param_count = param_count;
    func->instr_capacity = 64;
    func->instrs = calloc(func->instr_capacity, sizeof(TACInstr *));
    if (!func->instrs) {
        perror("calloc(instrs)");
        exit(1);
    }
    
    prog->funcs[prog->func_count++] = func;
    return func;
}

void tac_func_add_instr(TACFunc *func, TACInstr instr) {
    if (!func) return;
    
    if (func->instr_count >= func->instr_capacity) {
        func->instr_capacity *= 2;
        func->instrs = realloc(func->instrs, func->instr_capacity * sizeof(TACInstr *));
        if (!func->instrs) {
            perror("realloc(instrs)");
            exit(1);
        }
    }
    
    TACInstr *copy = malloc(sizeof(TACInstr));
    if (!copy) {
        perror("malloc(TACInstr)");
        exit(1);
    }
    *copy = instr;
    func->instrs[func->instr_count++] = copy;
}

void tac_program_add_global(TACProgram *prog, TACInstr instr) {
    if (!prog) return;
    
    if (prog->global_count >= prog->global_capacity) {
        prog->global_capacity *= 2;
        prog->globals = realloc(prog->globals, prog->global_capacity * sizeof(TACInstr *));
        if (!prog->globals) {
            perror("realloc(globals)");
            exit(1);
        }
    }
    
    TACInstr *copy = malloc(sizeof(TACInstr));
    if (!copy) {
        perror("malloc(TACInstr)");
        exit(1);
    }
    *copy = instr;
    prog->globals[prog->global_count++] = copy;
}

/* =========================================================================
   Operand Printing (for debugging)
   ========================================================================= */

static void operand_print(const Operand *op, FILE *out) {
    if (!op) return;
    switch (op->kind) {
    case OPERAND_CONST_INT:
        fprintf(out, "%d", op->int_val);
        break;
    case OPERAND_CONST_FLOAT:
        fprintf(out, "%g", op->float_val);
        break;
    case OPERAND_CONST_BOOL:
        fprintf(out, "%s", op->int_val ? "true" : "false");
        break;
    case OPERAND_CONST_CHAR:
        fprintf(out, "'%c'", (char)op->int_val);
        break;
    case OPERAND_TEMP:
    case OPERAND_VAR:
    case OPERAND_LABEL:
        fprintf(out, "%s", op->name ? op->name : "?");
        break;
    case OPERAND_STRING:
        fprintf(out, "%s", op->str_val ? op->str_val : "?");
        break;
    default:
        fprintf(out, "?");
    }
}

/* =========================================================================
   TAC Printing (for debugging)
   ========================================================================= */

static const char *tac_op_name(TACOpKind op) {
    switch (op) {
    case TAC_VAR: return "var";
    case TAC_ASSIGN: return "assign";
    case TAC_ASSIGN_CONST: return "assign_const";
    case TAC_BINOP: return "binop";
    case TAC_UNOP: return "unop";
    case TAC_ARRAY_LOAD: return "array_load";
    case TAC_ARRAY_STORE: return "array_store";
    case TAC_MEMBER_LOAD: return "member_load";
    case TAC_MEMBER_STORE: return "member_store";
    case TAC_PARAM: return "param";
    case TAC_CALL: return "call";
    case TAC_LABEL: return "label";
    case TAC_JUMP: return "jump";
    case TAC_COND_JUMP: return "cond_jump";
    case TAC_RETURN: return "return";
    case TAC_BREAK: return "break";
    case TAC_CONTINUE: return "continue";
    default: return "unknown";
    }
}

void tac_print(TACProgram *prog, FILE *out) {
    if (!prog) return;
    
    fprintf(out, "=== Structured TAC Program ===\n\n");
    
    /* Print globals */
    if (prog->global_count > 0) {
        fprintf(out, "=== Global Declarations ===\n");
        for (int i = 0; i < prog->global_count; i++) {
            TACInstr *instr = prog->globals[i];
            fprintf(out, "[%s] ", tac_op_name(instr->op));
            if (instr->op == TAC_VAR) {
                operand_print(&instr->dest, out);
                if (instr->attr_int > 0) fprintf(out, "[%d]", instr->attr_int);
            }
            fprintf(out, "\n");
        }
        fprintf(out, "\n");
    }
    
    /* Print functions */
    for (int i = 0; i < prog->func_count; i++) {
        TACFunc *func = prog->funcs[i];
        fprintf(out, "func %s (%d params):\n", func->name, func->param_count);
        
        for (int j = 0; j < func->instr_count; j++) {
            TACInstr *instr = prog->funcs[i]->instrs[j];
            fprintf(out, "  [%3d] ", j);
            
            switch (instr->op) {
            case TAC_VAR:
                fprintf(out, "var ");
                operand_print(&instr->dest, out);
                if (instr->attr_int > 0) fprintf(out, "[%d]", instr->attr_int);
                break;
            case TAC_ASSIGN:
                operand_print(&instr->dest, out);
                fprintf(out, " = ");
                operand_print(&instr->left, out);
                break;
            case TAC_ASSIGN_CONST:
                operand_print(&instr->dest, out);
                fprintf(out, " = ");
                operand_print(&instr->left, out);
                break;
            case TAC_BINOP:
                operand_print(&instr->dest, out);
                fprintf(out, " = ");
                operand_print(&instr->left, out);
                fprintf(out, " %s ", instr->attr ? instr->attr : "?");
                operand_print(&instr->right, out);
                break;
            case TAC_UNOP:
                operand_print(&instr->dest, out);
                fprintf(out, " = %s ", instr->attr ? instr->attr : "?");
                operand_print(&instr->left, out);
                break;
            case TAC_ARRAY_LOAD:
                operand_print(&instr->dest, out);
                fprintf(out, " = ");
                operand_print(&instr->left, out);
                fprintf(out, "[");
                operand_print(&instr->right, out);
                fprintf(out, "]");
                break;
            case TAC_ARRAY_STORE:
                operand_print(&instr->left, out);
                fprintf(out, "[");
                operand_print(&instr->right, out);
                fprintf(out, "] = ");
                operand_print(&instr->dest, out);
                break;
            case TAC_PARAM:
                fprintf(out, "param ");
                operand_print(&instr->left, out);
                break;
            case TAC_CALL:
                operand_print(&instr->dest, out);
                fprintf(out, " = call %s, %d", instr->attr ? instr->attr : "?", instr->attr_int);
                break;
            case TAC_LABEL:
                operand_print(&instr->dest, out);
                fprintf(out, ":");
                break;
            case TAC_JUMP:
                fprintf(out, "goto ");
                operand_print(&instr->dest, out);
                break;
            case TAC_COND_JUMP:
                fprintf(out, "if ");
                operand_print(&instr->left, out);
                fprintf(out, " goto ");
                operand_print(&instr->dest, out);
                break;
            case TAC_RETURN:
                fprintf(out, "return");
                if (instr->left.kind != OPERAND_INVALID) {
                    fprintf(out, " ");
                    operand_print(&instr->left, out);
                }
                break;
            case TAC_BREAK:
                fprintf(out, "break");
                break;
            case TAC_CONTINUE:
                fprintf(out, "continue");
                break;
            default:
                fprintf(out, "???");
            }
            fprintf(out, "\n");
        }
        fprintf(out, "endfunc %s\n\n", func->name);
    }
}
