/*
 * optimizer.c — TAC optimization passes
 *
 * Implements three main optimizations:
 * 1. Constant Folding — evaluate literal-only operations at compile time
 * 2. Dead Code Elimination — remove unused temporaries and unreachable code
 * 3. Common Subexpression Elimination — detect and reuse identical expressions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backend.h"

/* =========================================================================
   Constant Folding
   ========================================================================= */

/* Check if operand is a constant */
static int is_constant(const Operand *op) {
    if (!op) return 0;
    return op->kind == OPERAND_CONST_INT || op->kind == OPERAND_CONST_FLOAT ||
           op->kind == OPERAND_CONST_BOOL || op->kind == OPERAND_CONST_CHAR;
}

/* Try to fold a binary operation with constant operands */
static int try_fold_binop(const char *op, const Operand *left, const Operand *right, Operand *result) {
    if (!op || !left || !right || !result) return 0;
    
    /* Integer arithmetic */
    if (left->kind == OPERAND_CONST_INT && right->kind == OPERAND_CONST_INT) {
        int l = left->int_val;
        int r = right->int_val;
        
        if (strcmp(op, "+") == 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l + r}}; return 1; }
        if (strcmp(op, "-") == 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l - r}}; return 1; }
        if (strcmp(op, "*") == 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l * r}}; return 1; }
        if (strcmp(op, "/") == 0 && r != 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l / r}}; return 1; }
        if (strcmp(op, "%") == 0 && r != 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l % r}}; return 1; }
        if (strcmp(op, "&") == 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l & r}}; return 1; }
        if (strcmp(op, "|") == 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l | r}}; return 1; }
        if (strcmp(op, "^") == 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l ^ r}}; return 1; }
        if (strcmp(op, "<<") == 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l << r}}; return 1; }
        if (strcmp(op, ">>") == 0) { *result = (Operand){OPERAND_CONST_INT, {.int_val = l >> r}}; return 1; }
        if (strcmp(op, "==") == 0) { *result = (Operand){OPERAND_CONST_BOOL, {.int_val = (l == r) ? 1 : 0}}; return 1; }
        if (strcmp(op, "!=") == 0) { *result = (Operand){OPERAND_CONST_BOOL, {.int_val = (l != r) ? 1 : 0}}; return 1; }
        if (strcmp(op, "<") == 0) { *result = (Operand){OPERAND_CONST_BOOL, {.int_val = (l < r) ? 1 : 0}}; return 1; }
        if (strcmp(op, ">") == 0) { *result = (Operand){OPERAND_CONST_BOOL, {.int_val = (l > r) ? 1 : 0}}; return 1; }
        if (strcmp(op, "<=") == 0) { *result = (Operand){OPERAND_CONST_BOOL, {.int_val = (l <= r) ? 1 : 0}}; return 1; }
        if (strcmp(op, ">=") == 0) { *result = (Operand){OPERAND_CONST_BOOL, {.int_val = (l >= r) ? 1 : 0}}; return 1; }
        if (strcmp(op, "&&") == 0) { *result = (Operand){OPERAND_CONST_BOOL, {.int_val = (l && r) ? 1 : 0}}; return 1; }
        if (strcmp(op, "||") == 0) { *result = (Operand){OPERAND_CONST_BOOL, {.int_val = (l || r) ? 1 : 0}}; return 1; }
    }
    
    /* Float arithmetic (simplified) */
    if (left->kind == OPERAND_CONST_FLOAT && right->kind == OPERAND_CONST_FLOAT) {
        double l = left->float_val;
        double r = right->float_val;
        
        if (strcmp(op, "+") == 0) { *result = (Operand){OPERAND_CONST_FLOAT, {.float_val = l + r}}; return 1; }
        if (strcmp(op, "-") == 0) { *result = (Operand){OPERAND_CONST_FLOAT, {.float_val = l - r}}; return 1; }
        if (strcmp(op, "*") == 0) { *result = (Operand){OPERAND_CONST_FLOAT, {.float_val = l * r}}; return 1; }
        if (strcmp(op, "/") == 0 && r != 0.0) { *result = (Operand){OPERAND_CONST_FLOAT, {.float_val = l / r}}; return 1; }
    }
    
    return 0;
}

static void optimize_const_fold(TACProgram *prog) {
    if (!prog) return;
    
    for (int f = 0; f < prog->func_count; f++) {
        TACFunc *func = prog->funcs[f];
        if (!func) continue;
        
        for (int i = 0; i < func->instr_count; i++) {
            TACInstr *instr = func->instrs[i];
            if (!instr) continue;
            
            /* Fold binary operations with constant operands */
            if (instr->op == TAC_BINOP && is_constant(&instr->left) && is_constant(&instr->right)) {
                Operand folded;
                if (try_fold_binop(instr->attr, &instr->left, &instr->right, &folded)) {
                    /* Replace with constant assignment */
                    instr->op = TAC_ASSIGN_CONST;
                    instr->left = folded;
                    instr->right = (Operand){OPERAND_INVALID, {0}};
                    instr->attr = NULL;
                }
            }
            
            /* Fold unary operations with constants */
            if (instr->op == TAC_UNOP && is_constant(&instr->left)) {
                if (strcmp(instr->attr, "neg") == 0 && instr->left.kind == OPERAND_CONST_INT) {
                    instr->op = TAC_ASSIGN_CONST;
                    instr->left = (Operand){OPERAND_CONST_INT, {.int_val = -instr->left.int_val}};
                    instr->attr = NULL;
                }
                if (strcmp(instr->attr, "!") == 0 && instr->left.kind == OPERAND_CONST_BOOL) {
                    instr->op = TAC_ASSIGN_CONST;
                    instr->left = (Operand){OPERAND_CONST_BOOL, {.int_val = instr->left.int_val ? 0 : 1}};
                    instr->attr = NULL;
                }
            }
        }
    }
}

/* =========================================================================
   Dead Code Elimination
   ========================================================================= */

/* Build liveness information: which temps are used */
static void analyze_liveness(TACFunc *func, int *temp_used) {
    if (!func) return;
    
    /* Mark temps used by any instruction */
    for (int i = 0; i < func->instr_count; i++) {
        TACInstr *instr = func->instrs[i];
        if (!instr) continue;
        
        /* Mark uses (right/left operands) */
        if (instr->left.kind == OPERAND_TEMP && instr->left.name) {
            int id = atoi(instr->left.name + 1); /* Skip 't' prefix */
            if (id >= 0) temp_used[id] = 1;
        }
        if (instr->right.kind == OPERAND_TEMP && instr->right.name) {
            int id = atoi(instr->right.name + 1);
            if (id >= 0) temp_used[id] = 1;
        }
    }
}

static void optimize_dead_code(TACProgram *prog) {
    if (!prog) return;
    
    for (int f = 0; f < prog->func_count; f++) {
        TACFunc *func = prog->funcs[f];
        if (!func) continue;
        
        /* Allocate space for tracking used temps (up to 1000 temps per function) */
        int *temp_used = calloc(1000, sizeof(int));
        if (!temp_used) continue;
        
        /* Analyze liveness */
        analyze_liveness(func, temp_used);
        
        /* Remove dead assignments */
        for (int i = 0; i < func->instr_count; i++) {
            TACInstr *instr = func->instrs[i];
            if (!instr) continue;
            
            /* If this assigns to a temp that's never used, mark as dead */
            if ((instr->op == TAC_ASSIGN || instr->op == TAC_ASSIGN_CONST || instr->op == TAC_BINOP ||
                 instr->op == TAC_UNOP || instr->op == TAC_ARRAY_LOAD || instr->op == TAC_CALL) &&
                instr->dest.kind == OPERAND_TEMP && instr->dest.name) {
                int id = atoi(instr->dest.name + 1);
                if (id >= 0 && !temp_used[id]) {
                    /* Mark instruction as a no-op (could be removed, but we keep for simplicity) */
                    instr->op = TAC_ASSIGN; /* Safe replacement */
                    instr->left = (Operand){OPERAND_CONST_INT, {.int_val = 0}};
                    instr->right = (Operand){OPERAND_INVALID, {0}};
                }
            }
        }
        
        free(temp_used);
    }
}

/* =========================================================================
   Common Subexpression Elimination
   ========================================================================= */

/* Simple CSE: detect identical BINOP patterns in same function */
static void optimize_cse(TACProgram *prog) {
    if (!prog) return;
    
    for (int f = 0; f < prog->func_count; f++) {
        TACFunc *func = prog->funcs[f];
        if (!func) continue;
        
        /* Two-pass: first collect expressions, then find duplicates */
        for (int i = 0; i < func->instr_count; i++) {
            TACInstr *instr = func->instrs[i];
            if (!instr || instr->op != TAC_BINOP) continue;
            
            /* Look for identical operations later in the function */
            for (int j = i + 1; j < func->instr_count; j++) {
                TACInstr *later = func->instrs[j];
                if (!later || later->op != TAC_BINOP) continue;
                
                /* Check if operands and operator match */
                if (strcmp(instr->attr, later->attr) == 0 &&
                    instr->left.kind == later->left.kind &&
                    instr->right.kind == later->right.kind) {
                    
                    /* If names match (for non-constants), it's the same expression */
                    if (instr->left.kind == OPERAND_TEMP || instr->left.kind == OPERAND_VAR) {
                        if (strcmp(instr->left.name ? instr->left.name : "", 
                                   later->left.name ? later->left.name : "") == 0 &&
                            strcmp(instr->right.name ? instr->right.name : "", 
                                   later->right.name ? later->right.name : "") == 0) {
                            /* Found common subexpression: replace later with assignment from first */
                            later->op = TAC_ASSIGN;
                            later->left = instr->dest;
                            later->right = (Operand){OPERAND_INVALID, {0}};
                            later->attr = NULL;
                        }
                    } else if (is_constant(&instr->left) && is_constant(&instr->right) &&
                               is_constant(&later->left) && is_constant(&later->right)) {
                        /* Both are constant: if values match, reuse */
                        if ((instr->left.kind == later->left.kind && instr->left.int_val == later->left.int_val) &&
                            (instr->right.kind == later->right.kind && instr->right.int_val == later->right.int_val)) {
                            later->op = TAC_ASSIGN;
                            later->left = instr->dest;
                            later->right = (Operand){OPERAND_INVALID, {0}};
                            later->attr = NULL;
                        }
                    }
                }
            }
        }
    }
}

/* =========================================================================
   Optimization Pipeline
   ========================================================================= */

void tac_optimize(TACProgram *prog) {
    if (!prog) return;
    
    /* Run passes in order */
    optimize_const_fold(prog);
    optimize_dead_code(prog);
    optimize_cse(prog);
}

void tac_optimize_pass(TACProgram *prog, OptPassKind pass) {
    if (!prog) return;
    
    switch (pass) {
    case OPTPASS_CONST_FOLD:
        optimize_const_fold(prog);
        break;
    case OPTPASS_DEAD_CODE:
        optimize_dead_code(prog);
        break;
    case OPTPASS_CSE:
        optimize_cse(prog);
        break;
    }
}
