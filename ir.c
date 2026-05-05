#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ir.h"

typedef struct {
    FILE *out;
    int temp_id;
    int label_id;
} IRGen;

static char *xstrdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (!p) {
        perror("malloc");
        exit(1);
    }
    memcpy(p, s, n);
    return p;
}

static char *str_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    char *buf = malloc((size_t)len + 1);
    if (!buf) {
        perror("malloc");
        exit(1);
    }
    va_start(ap, fmt);
    vsnprintf(buf, (size_t)len + 1, fmt, ap);
    va_end(ap);
    return buf;
}

static void emit(IRGen *g, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(g->out, fmt, ap);
    va_end(ap);
    fputc('\n', g->out);
}

static char *new_temp(IRGen *g) { return str_printf("t%d", g->temp_id++); }
static char *new_label(IRGen *g) { return str_printf("L%d", g->label_id++); }

static char *gen_expr(IRGen *g, ASTNode *n);
static void gen_stmt(IRGen *g, ASTNode *n);

static char *lvalue_to_str(IRGen *g, ASTNode *n) {
    switch (n->kind) {
    case NODE_IDENT:
        return xstrdup(n->ident.name);
    case NODE_INDEX: {
        char *arr = gen_expr(g, n->index.array);
        char *idx = gen_expr(g, n->index.index);
        char *lv = str_printf("%s[%s]", arr, idx);
        return lv;
    }
    case NODE_MEMBER: {
        char *obj = gen_expr(g, n->member.object);
        char *lv = str_printf("%s%s%s", obj, n->member.arrow ? "->" : ".", n->member.field);
        return lv;
    }
    default:
        return xstrdup("<invalid-lvalue>");
    }
}

static char *gen_call(IRGen *g, ASTNode *n) {
    for (int i = 0; i < n->call.args->count; i++) {
        char *arg = gen_expr(g, n->call.args->items[i]);
        emit(g, "param %s", arg);
    }
    char *callee = gen_expr(g, n->call.callee);
    char *ret = new_temp(g);
    emit(g, "%s = call %s, %d", ret, callee, n->call.args->count);
    return ret;
}

static char *gen_expr(IRGen *g, ASTNode *n) {
    if (!n) return xstrdup("0");

    switch (n->kind) {
    case NODE_INT_LIT: return str_printf("%d", n->int_lit.ival);
    case NODE_FLOAT_LIT: return str_printf("%g", n->float_lit.fval);
    case NODE_CHAR_LIT: return str_printf("'%c'", n->char_lit.cval);
    case NODE_BOOL_LIT: return str_printf("%s", n->bool_lit.bval ? "true" : "false");
    case NODE_STR_LIT: return xstrdup(n->str_lit.sval);
    case NODE_IDENT: return xstrdup(n->ident.name);
    case NODE_CALL: return gen_call(g, n);
    case NODE_INDEX: {
        char *lv = lvalue_to_str(g, n);
        char *t = new_temp(g);
        emit(g, "%s = %s", t, lv);
        return t;
    }
    case NODE_MEMBER: {
        char *lv = lvalue_to_str(g, n);
        char *t = new_temp(g);
        emit(g, "%s = %s", t, lv);
        return t;
    }
    case NODE_CAST: {
        char *v = gen_expr(g, n->cast.expr);
        char *t = new_temp(g);
        const char *ty = (n->cast.type->kind == NODE_TYPE) ? n->cast.type->type.name : "struct";
        emit(g, "%s = (%s) %s", t, ty, v);
        return t;
    }
    case NODE_SIZEOF: {
        char *t = new_temp(g);
        emit(g, "%s = sizeof(...)", t);
        return t;
    }
    case NODE_UNOP: {
        if (strcmp(n->unop.op, "post++") == 0 || strcmp(n->unop.op, "post--") == 0 ||
            strcmp(n->unop.op, "pre++") == 0 || strcmp(n->unop.op, "pre--") == 0) {
            char *lv = lvalue_to_str(g, n->unop.operand);
            char *old = new_temp(g);
            emit(g, "%s = %s", old, lv);
            if (strstr(n->unop.op, "++")) emit(g, "%s = %s + 1", lv, lv);
            else emit(g, "%s = %s - 1", lv, lv);
            if (strncmp(n->unop.op, "post", 4) == 0) return old;
            char *now = new_temp(g);
            emit(g, "%s = %s", now, lv);
            return now;
        }
        char *v = gen_expr(g, n->unop.operand);
        char *t = new_temp(g);
        emit(g, "%s = %s %s", t, n->unop.op, v);
        return t;
    }
    case NODE_TERNARY: {
        char *cond = gen_expr(g, n->ternary.cond);
        char *l_true = new_label(g);
        char *l_false = new_label(g);
        char *l_end = new_label(g);
        char *res = new_temp(g);
        emit(g, "if %s goto %s", cond, l_true);
        emit(g, "goto %s", l_false);
        emit(g, "%s:", l_true);
        char *a = gen_expr(g, n->ternary.then_expr);
        emit(g, "%s = %s", res, a);
        emit(g, "goto %s", l_end);
        emit(g, "%s:", l_false);
        char *b = gen_expr(g, n->ternary.else_expr);
        emit(g, "%s = %s", res, b);
        emit(g, "%s:", l_end);
        return res;
    }
    case NODE_BINOP: {
        const char *op = n->binop.op;
        if (strcmp(op, "=") == 0 || strcmp(op, "+=") == 0 || strcmp(op, "-=") == 0 ||
            strcmp(op, "*=") == 0 || strcmp(op, "/=") == 0 || strcmp(op, "%=") == 0 ||
            strcmp(op, "&=") == 0 || strcmp(op, "|=") == 0 || strcmp(op, "^=") == 0 ||
            strcmp(op, "<<=") == 0 || strcmp(op, ">>=") == 0) {
            char *lv = lvalue_to_str(g, n->binop.left);
            char *rv = gen_expr(g, n->binop.right);
            if (strcmp(op, "=") == 0) emit(g, "%s = %s", lv, rv);
            else {
                char bop[4] = {0};
                strncpy(bop, op, strlen(op) - 1);
                emit(g, "%s = %s %s %s", lv, lv, bop, rv);
            }
            char *ret = new_temp(g);
            emit(g, "%s = %s", ret, lv);
            return ret;
        }

        char *l = gen_expr(g, n->binop.left);
        char *r = gen_expr(g, n->binop.right);
        char *t = new_temp(g);
        emit(g, "%s = %s %s %s", t, l, op, r);
        return t;
    }
    default:
        return xstrdup("0");
    }
}

static void gen_var_decl(IRGen *g, ASTNode *n) {
    if (n->var_decl.arr_size > 0) {
        emit(g, "var %s[%d]", n->var_decl.name, n->var_decl.arr_size);
    } else {
        emit(g, "var %s", n->var_decl.name);
    }
    if (n->var_decl.init) {
        char *rhs = gen_expr(g, n->var_decl.init);
        emit(g, "%s = %s", n->var_decl.name, rhs);
    }
}

static void gen_array_decl(IRGen *g, ASTNode *n) {
    emit(g, "var %s[%d]", n->arr_decl.name, n->arr_decl.size);
    for (int i = 0; i < n->arr_decl.init_list->count; i++) {
        char *v = gen_expr(g, n->arr_decl.init_list->items[i]);
        emit(g, "%s[%d] = %s", n->arr_decl.name, i, v);
    }
}

static void gen_stmt(IRGen *g, ASTNode *n) {
    if (!n) return;
    switch (n->kind) {
    case NODE_VAR_DECL: gen_var_decl(g, n); break;
    case NODE_ARRAY_DECL: gen_array_decl(g, n); break;
    case NODE_EXPR_STMT: (void)gen_expr(g, n->expr_stmt.expr); break;
    case NODE_EMPTY_STMT: break;
    case NODE_RETURN:
        if (n->ret_stmt.value) {
            char *v = gen_expr(g, n->ret_stmt.value);
            emit(g, "return %s", v);
        } else {
            emit(g, "return");
        }
        break;
    case NODE_IF: {
        char *cond = gen_expr(g, n->if_stmt.cond);
        char *l_then = new_label(g);
        char *l_else = new_label(g);
        char *l_end = new_label(g);
        emit(g, "if %s goto %s", cond, l_then);
        emit(g, "goto %s", l_else);
        emit(g, "%s:", l_then);
        gen_stmt(g, n->if_stmt.then_br);
        emit(g, "goto %s", l_end);
        emit(g, "%s:", l_else);
        if (n->if_stmt.else_br) gen_stmt(g, n->if_stmt.else_br);
        emit(g, "%s:", l_end);
        break;
    }
    case NODE_WHILE: {
        char *l_head = new_label(g);
        char *l_body = new_label(g);
        char *l_end = new_label(g);
        emit(g, "%s:", l_head);
        char *cond = gen_expr(g, n->while_stmt.cond);
        emit(g, "if %s goto %s", cond, l_body);
        emit(g, "goto %s", l_end);
        emit(g, "%s:", l_body);
        gen_stmt(g, n->while_stmt.body);
        emit(g, "goto %s", l_head);
        emit(g, "%s:", l_end);
        break;
    }
    case NODE_DO_WHILE: {
        char *l_body = new_label(g);
        char *l_end = new_label(g);
        emit(g, "%s:", l_body);
        gen_stmt(g, n->do_while_stmt.body);
        char *cond = gen_expr(g, n->do_while_stmt.cond);
        emit(g, "if %s goto %s", cond, l_body);
        emit(g, "%s:", l_end);
        break;
    }
    case NODE_FOR: {
        char *l_head = new_label(g);
        char *l_body = new_label(g);
        char *l_step = new_label(g);
        char *l_end = new_label(g);
        if (n->for_stmt.init) gen_stmt(g, n->for_stmt.init);
        emit(g, "%s:", l_head);
        if (n->for_stmt.cond) {
            char *cond = gen_expr(g, n->for_stmt.cond);
            emit(g, "if %s goto %s", cond, l_body);
            emit(g, "goto %s", l_end);
        } else {
            emit(g, "goto %s", l_body);
        }
        emit(g, "%s:", l_body);
        gen_stmt(g, n->for_stmt.body);
        emit(g, "%s:", l_step);
        if (n->for_stmt.incr) (void)gen_expr(g, n->for_stmt.incr);
        emit(g, "goto %s", l_head);
        emit(g, "%s:", l_end);
        break;
    }
    case NODE_BREAK:
        emit(g, "break");
        break;
    case NODE_CONTINUE:
        emit(g, "continue");
        break;
    case NODE_COMPOUND:
        for (int i = 0; i < n->compound.stmts->count; i++) gen_stmt(g, n->compound.stmts->items[i]);
        break;
    default:
        break;
    }
}

static void gen_function(IRGen *g, ASTNode *fn) {
    emit(g, "\nfunc %s:", fn->func_def.name);
    for (int i = 0; i < fn->func_def.params->count; i++) {
        ASTNode *p = fn->func_def.params->items[i];
        emit(g, "param_def %s", p->param.name);
    }
    gen_stmt(g, fn->func_def.body);
    emit(g, "endfunc %s", fn->func_def.name);
}

void ir_generate(ASTNode *root, FILE *out) {
    if (!root || root->kind != NODE_PROGRAM) return;
    IRGen g = {0};
    g.out = out;
    emit(&g, "=== Three Address Code (TAC) ===");
    for (int i = 0; i < root->program.decls->count; i++) {
        ASTNode *d = root->program.decls->items[i];
        if (d->kind == NODE_FUNC_DEF) gen_function(&g, d);
        else if (d->kind == NODE_VAR_DECL) gen_var_decl(&g, d);
        else if (d->kind == NODE_ARRAY_DECL) gen_array_decl(&g, d);
    }
}
