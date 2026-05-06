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

static void operand_brief(const Operand *op, char *buf, size_t n) {
    if (!op || !buf || n == 0) return;
    switch (op->kind) {
    case OPERAND_CONST_INT: snprintf(buf, n, "%d", op->int_val); break;
    case OPERAND_CONST_FLOAT: snprintf(buf, n, "%g", op->float_val); break;
    case OPERAND_CONST_BOOL: snprintf(buf, n, "%s", op->int_val ? "true" : "false"); break;
    case OPERAND_CONST_CHAR: snprintf(buf, n, "'%c'", (char)op->int_val); break;
    case OPERAND_TEMP:
    case OPERAND_VAR:
    case OPERAND_LABEL: snprintf(buf, n, "%s", op->name ? op->name : "?"); break;
    case OPERAND_STRING: snprintf(buf, n, "\"%s\"", op->str_val ? op->str_val : ""); break;
    default: snprintf(buf, n, "_"); break;
    }
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
                    char d[64], l[64], r[64], res[64];
                    operand_brief(&instr->dest, d, sizeof(d));
                    operand_brief(&instr->left, l, sizeof(l));
                    operand_brief(&instr->right, r, sizeof(r));
                    operand_brief(&folded, res, sizeof(res));
                    fprintf(stderr,
                            "[CONST_FOLD] %s: instr[%d] %s = %s %s %s -> %s = %s\n",
                            func->name ? func->name : "<anon>",
                            i,
                            d,
                            l,
                            instr->attr ? instr->attr : "?",
                            r,
                            d,
                            res);
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
                    char d[64], l[64];
                    operand_brief(&instr->dest, d, sizeof(d));
                    operand_brief(&instr->left, l, sizeof(l));
                    fprintf(stderr,
                            "[CONST_FOLD] %s: instr[%d] %s = neg %s -> %s = %d\n",
                            func->name ? func->name : "<anon>",
                            i,
                            d,
                            l,
                            d,
                            -instr->left.int_val);
                    instr->op = TAC_ASSIGN_CONST;
                    instr->left = (Operand){OPERAND_CONST_INT, {.int_val = -instr->left.int_val}};
                    instr->attr = NULL;
                }
                if (strcmp(instr->attr, "!") == 0 && instr->left.kind == OPERAND_CONST_BOOL) {
                    char d[64], l[64];
                    operand_brief(&instr->dest, d, sizeof(d));
                    operand_brief(&instr->left, l, sizeof(l));
                    fprintf(stderr,
                            "[CONST_FOLD] %s: instr[%d] %s = ! %s -> %s = %s\n",
                            func->name ? func->name : "<anon>",
                            i,
                            d,
                            l,
                            d,
                            instr->left.int_val ? "false" : "true");
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

typedef struct {
    char **items;
    int count;
    int capacity;
} NameSet;

static void nameset_init(NameSet *set, int capacity_hint) {
    set->count = 0;
    set->capacity = capacity_hint > 0 ? capacity_hint : 8;
    set->items = malloc((size_t)set->capacity * sizeof(char *));
}

static void nameset_free(NameSet *set) {
    if (!set) return;
    free(set->items);
    set->items = NULL;
    set->count = 0;
    set->capacity = 0;
}

static int nameset_contains(const NameSet *set, const char *name) {
    if (!set || !name) return 0;
    for (int i = 0; i < set->count; i++) {
        if (strcmp(set->items[i], name) == 0) return 1;
    }
    return 0;
}

static void nameset_add(NameSet *set, const char *name) {
    if (!set || !name || nameset_contains(set, name)) return;
    if (set->count >= set->capacity) {
        set->capacity *= 2;
        set->items = realloc(set->items, (size_t)set->capacity * sizeof(char *));
        if (!set->items) {
            perror("realloc(nameset)");
            exit(1);
        }
    }
    set->items[set->count++] = (char *)name;
}

static void nameset_remove(NameSet *set, const char *name) {
    if (!set || !name) return;
    for (int i = 0; i < set->count; i++) {
        if (strcmp(set->items[i], name) == 0) {
            set->items[i] = set->items[set->count - 1];
            set->count--;
            return;
        }
    }
}

static const char *op_name(TACOpKind op) {
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

static void operand_to_string(const Operand *op, char *buf, size_t n) {
    if (!op || !buf || n == 0) return;
    switch (op->kind) {
    case OPERAND_CONST_INT: snprintf(buf, n, "%d", op->int_val); break;
    case OPERAND_CONST_FLOAT: snprintf(buf, n, "%g", op->float_val); break;
    case OPERAND_CONST_BOOL: snprintf(buf, n, "%s", op->int_val ? "true" : "false"); break;
    case OPERAND_CONST_CHAR: snprintf(buf, n, "'%c'", (char)op->int_val); break;
    case OPERAND_TEMP:
    case OPERAND_VAR:
    case OPERAND_LABEL: snprintf(buf, n, "%s", op->name ? op->name : "?"); break;
    case OPERAND_STRING: snprintf(buf, n, "\"%s\"", op->str_val ? op->str_val : ""); break;
    default: snprintf(buf, n, "_"); break;
    }
}

static int writes_destination(const TACInstr *instr) {
    if (!instr) return 0;
    switch (instr->op) {
    case TAC_ASSIGN:
    case TAC_ASSIGN_CONST:
    case TAC_BINOP:
    case TAC_UNOP:
    case TAC_ARRAY_LOAD:
    case TAC_MEMBER_LOAD:
    case TAC_CALL:
        return 1;
    default:
        return 0;
    }
}

static int dce_eliminable(const TACInstr *instr) {
    if (!instr) return 0;
    switch (instr->op) {
    case TAC_ASSIGN:
    case TAC_ASSIGN_CONST:
    case TAC_BINOP:
    case TAC_UNOP:
    case TAC_ARRAY_LOAD:
    case TAC_MEMBER_LOAD:
        return 1;
    default:
        return 0;
    }
}

static void mark_operand_live(const Operand *op, NameSet *live_temps, NameSet *live_vars) {
    if (!op) return;
    if (op->kind == OPERAND_TEMP && op->name) nameset_add(live_temps, op->name);
    if (op->kind == OPERAND_VAR && op->name) nameset_add(live_vars, op->name);
}

static void optimize_dead_code(TACProgram *prog) {
    if (!prog) return;
    
    for (int f = 0; f < prog->func_count; f++) {
        TACFunc *func = prog->funcs[f];
        if (!func) continue;

        NameSet live_temps, live_vars;
        nameset_init(&live_temps, func->instr_count);
        nameset_init(&live_vars, func->instr_count);
        if (!live_temps.items || !live_vars.items) {
            nameset_free(&live_temps);
            nameset_free(&live_vars);
            continue;
        }

        int *dead = calloc((size_t)func->instr_count, sizeof(int));
        if (!dead) {
            nameset_free(&live_temps);
            nameset_free(&live_vars);
            continue;
        }

        int removed = 0;
        for (int i = func->instr_count - 1; i >= 0; i--) {
            TACInstr *instr = func->instrs[i];
            if (!instr) continue;

            if (dce_eliminable(instr) &&
                (instr->dest.kind == OPERAND_TEMP || instr->dest.kind == OPERAND_VAR) &&
                instr->dest.name) {
                int is_live = (instr->dest.kind == OPERAND_TEMP)
                                  ? nameset_contains(&live_temps, instr->dest.name)
                                  : nameset_contains(&live_vars, instr->dest.name);
                if (!is_live) {
                    char d[64], l[64], r[64];
                    operand_to_string(&instr->dest, d, sizeof(d));
                    operand_to_string(&instr->left, l, sizeof(l));
                    operand_to_string(&instr->right, r, sizeof(r));
                    fprintf(stderr,
                            "[DCE] %s: removed instr[%d]: op=%s dest=%s left=%s right=%s%s%s\n",
                            func->name ? func->name : "<anon>",
                            i,
                            op_name(instr->op),
                            d,
                            l,
                            r,
                            instr->attr ? " attr=" : "",
                            instr->attr ? instr->attr : "");
                    dead[i] = 1;
                    removed++;
                    continue;
                }
            }

            if (writes_destination(instr) &&
                (instr->dest.kind == OPERAND_TEMP || instr->dest.kind == OPERAND_VAR) &&
                instr->dest.name) {
                if (instr->dest.kind == OPERAND_TEMP) nameset_remove(&live_temps, instr->dest.name);
                else nameset_remove(&live_vars, instr->dest.name);
            }

            mark_operand_live(&instr->left, &live_temps, &live_vars);
            mark_operand_live(&instr->right, &live_temps, &live_vars);
        }

        if (removed > 0) {
            int w = 0;
            for (int r = 0; r < func->instr_count; r++) {
                if (dead[r]) {
                    free(func->instrs[r]);
                } else {
                    func->instrs[w++] = func->instrs[r];
                }
            }
            func->instr_count = w;
        }

        free(dead);
        nameset_free(&live_temps);
        nameset_free(&live_vars);
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
                if (instr->attr && later->attr &&
                    strcmp(instr->attr, later->attr) == 0 &&
                    instr->left.kind == later->left.kind &&
                    instr->right.kind == later->right.kind) {
                    
                    /* If names match (for non-constants), it's the same expression */
                    if (instr->left.kind == OPERAND_TEMP || instr->left.kind == OPERAND_VAR) {
                        if (strcmp(instr->left.name ? instr->left.name : "", 
                                   later->left.name ? later->left.name : "") == 0 &&
                            strcmp(instr->right.name ? instr->right.name : "", 
                                   later->right.name ? later->right.name : "") == 0) {
                            char ld[64], ll[64], lr[64], sd[64];
                            operand_to_string(&later->dest, ld, sizeof(ld));
                            operand_to_string(&later->left, ll, sizeof(ll));
                            operand_to_string(&later->right, lr, sizeof(lr));
                            operand_to_string(&instr->dest, sd, sizeof(sd));
                            fprintf(stderr,
                                    "[CSE] %s: instr[%d] %s = %s %s %s -> %s = %s (reuse instr[%d])\n",
                                    func->name ? func->name : "<anon>",
                                    j,
                                    ld,
                                    ll,
                                    later->attr ? later->attr : "?",
                                    lr,
                                    ld,
                                    sd,
                                    i);
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
                            char ld[64], ll[64], lr[64], sd[64];
                            operand_to_string(&later->dest, ld, sizeof(ld));
                            operand_to_string(&later->left, ll, sizeof(ll));
                            operand_to_string(&later->right, lr, sizeof(lr));
                            operand_to_string(&instr->dest, sd, sizeof(sd));
                            fprintf(stderr,
                                    "[CSE] %s: instr[%d] %s = %s %s %s -> %s = %s (reuse instr[%d])\n",
                                    func->name ? func->name : "<anon>",
                                    j,
                                    ld,
                                    ll,
                                    later->attr ? later->attr : "?",
                                    lr,
                                    ld,
                                    sd,
                                    i);
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
