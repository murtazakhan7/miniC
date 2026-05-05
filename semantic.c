#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

typedef enum {
    TY_INVALID,
    TY_VOID,
    TY_BOOL,
    TY_CHAR,
    TY_INT,
    TY_FLOAT,
    TY_STRING,
    TY_STRUCT,
    TY_ARRAY,
    TY_FUNCTION
} TypeKind;

typedef struct Type Type;
typedef struct Symbol Symbol;
typedef struct Scope Scope;
typedef struct StructDef StructDef;

struct Type {
    TypeKind kind;
    Type *base;
    char *name;
    int arr_size;
    Type *ret;
    Type **params;
    int param_count;
};

struct Symbol {
    char *name;
    Type *type;
    int line;
    Symbol *next;
};

struct Scope {
    Symbol *symbols;
    Scope *parent;
};

struct StructDef {
    char *name;
    Scope *members;
    StructDef *next;
};

typedef struct {
    Scope *scope;
    StructDef *structs;
    Type *current_ret_type;
    int loop_depth;
    int errors;
} SemCtx;

static Type TY_INVALID_OBJ = { TY_INVALID, NULL, NULL, 0, NULL, NULL, 0 };
static Type TY_VOID_OBJ = { TY_VOID, NULL, NULL, 0, NULL, NULL, 0 };
static Type TY_BOOL_OBJ = { TY_BOOL, NULL, NULL, 0, NULL, NULL, 0 };
static Type TY_CHAR_OBJ = { TY_CHAR, NULL, NULL, 0, NULL, NULL, 0 };
static Type TY_INT_OBJ = { TY_INT, NULL, NULL, 0, NULL, NULL, 0 };
static Type TY_FLOAT_OBJ = { TY_FLOAT, NULL, NULL, 0, NULL, NULL, 0 };
static Type TY_STRING_OBJ = { TY_STRING, NULL, NULL, 0, NULL, NULL, 0 };

static Type *ty_invalid(void) { return &TY_INVALID_OBJ; }
static Type *ty_void(void) { return &TY_VOID_OBJ; }
static Type *ty_bool(void) { return &TY_BOOL_OBJ; }
static Type *ty_char(void) { return &TY_CHAR_OBJ; }
static Type *ty_int(void) { return &TY_INT_OBJ; }
static Type *ty_float(void) { return &TY_FLOAT_OBJ; }
static Type *ty_string(void) { return &TY_STRING_OBJ; }

static void sem_error(SemCtx *ctx, int line, const char *msg, const char *name) {
    if (name) {
        fprintf(stderr, "[Semantic Error] Line %d: %s '%s'\n", line, msg, name);
    } else {
        fprintf(stderr, "[Semantic Error] Line %d: %s\n", line, msg);
    }
    ctx->errors++;
}

static Type *make_sem_type(TypeKind kind, Type *base, const char *name, int arr_size) {
    Type *t = calloc(1, sizeof(Type));
    if (!t) {
        perror("calloc(Type)");
        exit(1);
    }
    t->kind = kind;
    t->base = base;
    t->arr_size = arr_size;
    if (name) t->name = strdup(name);
    return t;
}

static Type *make_function_type(Type *ret, Type **params, int param_count) {
    Type *t = make_sem_type(TY_FUNCTION, NULL, NULL, 0);
    t->ret = ret;
    t->params = params;
    t->param_count = param_count;
    return t;
}

static Type *make_variadic_function_type(Type *ret) {
    return make_function_type(ret, NULL, -1);
}

static Scope *scope_push(Scope *parent) {
    Scope *s = calloc(1, sizeof(Scope));
    if (!s) {
        perror("calloc(Scope)");
        exit(1);
    }
    s->parent = parent;
    return s;
}

static Scope *scope_pop(Scope *scope) {
    Scope *parent = scope->parent;
    Symbol *sym = scope->symbols;
    while (sym) {
        Symbol *next = sym->next;
        free(sym);
        sym = next;
    }
    free(scope);
    return parent;
}

static Symbol *scope_lookup_local(Scope *s, const char *name) {
    Symbol *it = s ? s->symbols : NULL;
    while (it) {
        if (strcmp(it->name, name) == 0) return it;
        it = it->next;
    }
    return NULL;
}

static Symbol *scope_lookup(Scope *s, const char *name) {
    for (Scope *it = s; it; it = it->parent) {
        Symbol *hit = scope_lookup_local(it, name);
        if (hit) return hit;
    }
    return NULL;
}

static Symbol *scope_add(Scope *s, const char *name, Type *type, int line) {
    Symbol *sym = calloc(1, sizeof(Symbol));
    if (!sym) {
        perror("calloc(Symbol)");
        exit(1);
    }
    sym->name = (char *)name;
    sym->type = type;
    sym->line = line;
    sym->next = s->symbols;
    s->symbols = sym;
    return sym;
}

static StructDef *find_struct(SemCtx *ctx, const char *name) {
    StructDef *it = ctx->structs;
    while (it) {
        if (strcmp(it->name, name) == 0) return it;
        it = it->next;
    }
    return NULL;
}

static int type_equal(Type *a, Type *b) {
    if (a == b) return 1;
    if (!a || !b) return 0;
    if (a->kind != b->kind) return 0;
    if (a->kind == TY_STRUCT) return a->name && b->name && strcmp(a->name, b->name) == 0;
    if (a->kind == TY_ARRAY) return type_equal(a->base, b->base);
    return 1;
}

static int is_numeric(Type *t) {
    return t && (t->kind == TY_INT || t->kind == TY_FLOAT || t->kind == TY_CHAR || t->kind == TY_BOOL);
}

static int is_integer_like(Type *t) {
    return t && (t->kind == TY_INT || t->kind == TY_CHAR || t->kind == TY_BOOL);
}

static int is_scalar(Type *t) {
    if (!t) return 0;
    return is_numeric(t) || t->kind == TY_STRUCT || t->kind == TY_ARRAY;
}

static int can_assign(Type *dst, Type *src) {
    if (!dst || !src) return 0;
    if (dst->kind == TY_INVALID || src->kind == TY_INVALID) return 1;
    if (dst->kind == TY_ARRAY || dst->kind == TY_FUNCTION || dst->kind == TY_VOID) return 0;
    if (type_equal(dst, src)) return 1;
    if (is_numeric(dst) && is_numeric(src)) return 1;
    return 0;
}

static Type *type_from_ast(SemCtx *ctx, ASTNode *type_node) {
    if (!type_node) return ty_invalid();
    if (type_node->kind == NODE_TYPE) {
        if (strcmp(type_node->type.name, "int") == 0) return ty_int();
        if (strcmp(type_node->type.name, "float") == 0) return ty_float();
        if (strcmp(type_node->type.name, "char") == 0) return ty_char();
        if (strcmp(type_node->type.name, "bool") == 0) return ty_bool();
        if (strcmp(type_node->type.name, "void") == 0) return ty_void();
    }
    if (type_node->kind == NODE_STRUCT_TYPE) {
        StructDef *sd = find_struct(ctx, type_node->struct_type.name);
        if (!sd) {
            sem_error(ctx, type_node->line, "Unknown struct type", type_node->struct_type.name);
            return ty_invalid();
        }
        return make_sem_type(TY_STRUCT, NULL, type_node->struct_type.name, 0);
    }
    return ty_invalid();
}

static Type *semantic_expr(SemCtx *ctx, ASTNode *expr);
static void semantic_stmt(SemCtx *ctx, ASTNode *stmt);
static void semantic_decl(SemCtx *ctx, ASTNode *decl);

static Type *decay_array(Type *t) {
    if (t && t->kind == TY_ARRAY) return t->base;
    return t;
}

static Type *arithmetic_result(Type *a, Type *b) {
    if (!is_numeric(a) || !is_numeric(b)) return ty_invalid();
    if (a->kind == TY_FLOAT || b->kind == TY_FLOAT) return ty_float();
    return ty_int();
}

static int is_lvalue(ASTNode *expr) {
    if (!expr) return 0;
    return expr->kind == NODE_IDENT || expr->kind == NODE_INDEX || expr->kind == NODE_MEMBER;
}

static Type *semantic_call(SemCtx *ctx, ASTNode *call) {
    Type *callee = semantic_expr(ctx, call->call.callee);
    if (!callee || callee->kind != TY_FUNCTION) {
        sem_error(ctx, call->line, "Attempt to call a non-function expression", NULL);
        for (int i = 0; i < call->call.args->count; i++) (void)semantic_expr(ctx, call->call.args->items[i]);
        return ty_invalid();
    }

    if (callee->param_count >= 0 && call->call.args->count != callee->param_count) {
        sem_error(ctx, call->line, "Function call argument count mismatch", NULL);
    }

    int n = callee->param_count < 0
        ? 0
        : (call->call.args->count < callee->param_count ? call->call.args->count : callee->param_count);
    for (int i = 0; i < n; i++) {
        Type *arg = decay_array(semantic_expr(ctx, call->call.args->items[i]));
        Type *param = decay_array(callee->params[i]);
        if (!can_assign(param, arg)) {
            sem_error(ctx, call->call.args->items[i]->line, "Argument type mismatch", NULL);
        }
    }

    for (int i = n; i < call->call.args->count; i++) (void)semantic_expr(ctx, call->call.args->items[i]);
    return callee->ret;
}

static Type *semantic_member(SemCtx *ctx, ASTNode *member) {
    Type *obj = semantic_expr(ctx, member->member.object);
    if (!obj || obj->kind != TY_STRUCT) {
        sem_error(ctx, member->line, "Member access on non-struct type", NULL);
        return ty_invalid();
    }

    StructDef *sd = find_struct(ctx, obj->name);
    if (!sd) {
        sem_error(ctx, member->line, "Unknown struct type in member access", obj->name);
        return ty_invalid();
    }

    Symbol *field = scope_lookup_local(sd->members, member->member.field);
    if (!field) {
        sem_error(ctx, member->line, "Struct has no such field", member->member.field);
        return ty_invalid();
    }
    return field->type;
}

static Type *semantic_expr(SemCtx *ctx, ASTNode *expr) {
    if (!expr) return ty_void();
    switch (expr->kind) {
    case NODE_INT_LIT: return ty_int();
    case NODE_FLOAT_LIT: return ty_float();
    case NODE_CHAR_LIT: return ty_char();
    case NODE_BOOL_LIT: return ty_bool();
    case NODE_STR_LIT: return ty_string();
    case NODE_IDENT: {
        Symbol *sym = scope_lookup(ctx->scope, expr->ident.name);
        if (!sym) {
            sem_error(ctx, expr->line, "Use of undeclared identifier", expr->ident.name);
            return ty_invalid();
        }
        return sym->type;
    }
    case NODE_CALL:
        return semantic_call(ctx, expr);
    case NODE_INDEX: {
        Type *arr = semantic_expr(ctx, expr->index.array);
        Type *idx = semantic_expr(ctx, expr->index.index);
        if (!is_integer_like(idx)) sem_error(ctx, expr->line, "Array index must be an integer type", NULL);
        if (arr && arr->kind == TY_ARRAY) return arr->base;
        sem_error(ctx, expr->line, "Indexing requires an array expression", NULL);
        return ty_invalid();
    }
    case NODE_MEMBER:
        return semantic_member(ctx, expr);
    case NODE_CAST:
        (void)semantic_expr(ctx, expr->cast.expr);
        return type_from_ast(ctx, expr->cast.type);
    case NODE_SIZEOF:
        if (expr->sizeof_expr.is_type) (void)type_from_ast(ctx, expr->sizeof_expr.type_or_expr);
        else (void)semantic_expr(ctx, expr->sizeof_expr.type_or_expr);
        return ty_int();
    case NODE_UNOP: {
        Type *operand = semantic_expr(ctx, expr->unop.operand);
        if (strcmp(expr->unop.op, "!") == 0) {
            if (!is_scalar(operand)) sem_error(ctx, expr->line, "Logical not requires scalar operand", NULL);
            return ty_bool();
        }
        if (strcmp(expr->unop.op, "~") == 0) {
            if (!is_integer_like(operand)) sem_error(ctx, expr->line, "Bitwise not requires integer operand", NULL);
            return ty_int();
        }
        if (strcmp(expr->unop.op, "neg") == 0) {
            if (!is_numeric(operand)) sem_error(ctx, expr->line, "Unary minus requires numeric operand", NULL);
            return operand;
        }
        if (strcmp(expr->unop.op, "pre++") == 0 || strcmp(expr->unop.op, "pre--") == 0 ||
            strcmp(expr->unop.op, "post++") == 0 || strcmp(expr->unop.op, "post--") == 0) {
            if (!is_lvalue(expr->unop.operand) || !is_numeric(operand)) {
                sem_error(ctx, expr->line, "Increment/decrement requires numeric lvalue", NULL);
            }
            return operand;
        }
        if (strcmp(expr->unop.op, "addr") == 0 || strcmp(expr->unop.op, "deref") == 0) {
            return operand;
        }
        return ty_invalid();
    }
    case NODE_TERNARY: {
        Type *c = semantic_expr(ctx, expr->ternary.cond);
        Type *a = semantic_expr(ctx, expr->ternary.then_expr);
        Type *b = semantic_expr(ctx, expr->ternary.else_expr);
        if (!is_scalar(c)) sem_error(ctx, expr->line, "Ternary condition must be scalar", NULL);
        if (type_equal(a, b)) return a;
        if (is_numeric(a) && is_numeric(b)) return arithmetic_result(a, b);
        sem_error(ctx, expr->line, "Incompatible ternary branch types", NULL);
        return ty_invalid();
    }
    case NODE_BINOP: {
        Type *l = semantic_expr(ctx, expr->binop.left);
        Type *r = semantic_expr(ctx, expr->binop.right);
        const char *op = expr->binop.op;

        if (strcmp(op, "=") == 0 || strcmp(op, "+=") == 0 || strcmp(op, "-=") == 0 ||
            strcmp(op, "*=") == 0 || strcmp(op, "/=") == 0 || strcmp(op, "%=") == 0 ||
            strcmp(op, "&=") == 0 || strcmp(op, "|=") == 0 || strcmp(op, "^=") == 0 ||
            strcmp(op, "<<=") == 0 || strcmp(op, ">>=") == 0) {
            if (!is_lvalue(expr->binop.left)) sem_error(ctx, expr->line, "Left side of assignment must be assignable", NULL);
            if (!can_assign(decay_array(l), decay_array(r))) sem_error(ctx, expr->line, "Type mismatch in assignment", NULL);
            return l;
        }

        if (strcmp(op, "||") == 0 || strcmp(op, "&&") == 0) {
            if (!is_scalar(l) || !is_scalar(r)) sem_error(ctx, expr->line, "Logical operators require scalar operands", NULL);
            return ty_bool();
        }

        if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 ||
            strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
            strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) {
            if (!is_numeric(l) || !is_numeric(r)) sem_error(ctx, expr->line, "Comparison requires numeric operands", NULL);
            return ty_bool();
        }

        if (strcmp(op, "&") == 0 || strcmp(op, "|") == 0 || strcmp(op, "^") == 0 ||
            strcmp(op, "<<") == 0 || strcmp(op, ">>") == 0 || strcmp(op, "%") == 0) {
            if (!is_integer_like(l) || !is_integer_like(r)) sem_error(ctx, expr->line, "Bitwise/integer operator requires integer operands", NULL);
            return ty_int();
        }

        if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
            strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
            Type *res = arithmetic_result(l, r);
            if (res->kind == TY_INVALID) sem_error(ctx, expr->line, "Arithmetic operator requires numeric operands", NULL);
            return res;
        }
        return ty_invalid();
    }
    default:
        return ty_invalid();
    }
}

static void check_var_decl(SemCtx *ctx, ASTNode *decl) {
    Type *base = type_from_ast(ctx, decl->var_decl.type);
    Type *final = base;
    if (decl->var_decl.arr_size > 0) final = make_sem_type(TY_ARRAY, base, NULL, decl->var_decl.arr_size);

    if (scope_lookup_local(ctx->scope, decl->var_decl.name)) {
        sem_error(ctx, decl->line, "Redeclaration of identifier in same scope", decl->var_decl.name);
        return;
    }
    scope_add(ctx->scope, decl->var_decl.name, final, decl->line);

    if (decl->var_decl.init) {
        Type *rhs = decay_array(semantic_expr(ctx, decl->var_decl.init));
        if (!can_assign(decay_array(final), rhs)) sem_error(ctx, decl->line, "Type mismatch in variable initializer", decl->var_decl.name);
    }
}

static void check_array_decl(SemCtx *ctx, ASTNode *decl) {
    Type *base = type_from_ast(ctx, decl->arr_decl.type);
    Type *arr = make_sem_type(TY_ARRAY, base, NULL, decl->arr_decl.size);
    if (scope_lookup_local(ctx->scope, decl->arr_decl.name)) {
        sem_error(ctx, decl->line, "Redeclaration of identifier in same scope", decl->arr_decl.name);
        return;
    }
    scope_add(ctx->scope, decl->arr_decl.name, arr, decl->line);
    if (decl->arr_decl.init_list->count > decl->arr_decl.size) {
        sem_error(ctx, decl->line, "Too many initializers for array", decl->arr_decl.name);
    }
    for (int i = 0; i < decl->arr_decl.init_list->count; i++) {
        Type *rhs = semantic_expr(ctx, decl->arr_decl.init_list->items[i]);
        if (!can_assign(base, rhs)) sem_error(ctx, decl->arr_decl.init_list->items[i]->line, "Array initializer type mismatch", decl->arr_decl.name);
    }
}

static void semantic_stmt(SemCtx *ctx, ASTNode *stmt) {
    if (!stmt) return;
    switch (stmt->kind) {
    case NODE_VAR_DECL: check_var_decl(ctx, stmt); break;
    case NODE_ARRAY_DECL: check_array_decl(ctx, stmt); break;
    case NODE_EXPR_STMT: (void)semantic_expr(ctx, stmt->expr_stmt.expr); break;
    case NODE_EMPTY_STMT: break;
    case NODE_RETURN: {
        Type *ret = stmt->ret_stmt.value ? decay_array(semantic_expr(ctx, stmt->ret_stmt.value)) : ty_void();
        if (!can_assign(decay_array(ctx->current_ret_type), ret)) {
            sem_error(ctx, stmt->line, "Return type mismatch", NULL);
        }
        break;
    }
    case NODE_IF: {
        Type *cond = semantic_expr(ctx, stmt->if_stmt.cond);
        if (!is_scalar(cond)) sem_error(ctx, stmt->line, "If condition must be scalar", NULL);
        semantic_stmt(ctx, stmt->if_stmt.then_br);
        semantic_stmt(ctx, stmt->if_stmt.else_br);
        break;
    }
    case NODE_WHILE: {
        Type *cond = semantic_expr(ctx, stmt->while_stmt.cond);
        if (!is_scalar(cond)) sem_error(ctx, stmt->line, "While condition must be scalar", NULL);
        ctx->loop_depth++;
        semantic_stmt(ctx, stmt->while_stmt.body);
        ctx->loop_depth--;
        break;
    }
    case NODE_DO_WHILE: {
        ctx->loop_depth++;
        semantic_stmt(ctx, stmt->do_while_stmt.body);
        ctx->loop_depth--;
        Type *cond = semantic_expr(ctx, stmt->do_while_stmt.cond);
        if (!is_scalar(cond)) sem_error(ctx, stmt->line, "Do-while condition must be scalar", NULL);
        break;
    }
    case NODE_FOR: {
        ctx->scope = scope_push(ctx->scope);
        if (stmt->for_stmt.init) semantic_stmt(ctx, stmt->for_stmt.init);
        if (stmt->for_stmt.cond) {
            Type *cond = semantic_expr(ctx, stmt->for_stmt.cond);
            if (!is_scalar(cond)) sem_error(ctx, stmt->line, "For condition must be scalar", NULL);
        }
        if (stmt->for_stmt.incr) (void)semantic_expr(ctx, stmt->for_stmt.incr);
        ctx->loop_depth++;
        semantic_stmt(ctx, stmt->for_stmt.body);
        ctx->loop_depth--;
        ctx->scope = scope_pop(ctx->scope);
        break;
    }
    case NODE_BREAK:
        if (ctx->loop_depth == 0) sem_error(ctx, stmt->line, "break outside loop", NULL);
        break;
    case NODE_CONTINUE:
        if (ctx->loop_depth == 0) sem_error(ctx, stmt->line, "continue outside loop", NULL);
        break;
    case NODE_COMPOUND:
        ctx->scope = scope_push(ctx->scope);
        for (int i = 0; i < stmt->compound.stmts->count; i++) semantic_stmt(ctx, stmt->compound.stmts->items[i]);
        ctx->scope = scope_pop(ctx->scope);
        break;
    default:
        (void)semantic_expr(ctx, stmt);
        break;
    }
}

static void semantic_function(SemCtx *ctx, ASTNode *fn) {
    if (scope_lookup_local(ctx->scope, fn->func_def.name)) {
        sem_error(ctx, fn->line, "Redeclaration of function", fn->func_def.name);
        return;
    }

    Type **params = NULL;
    if (fn->func_def.params->count > 0) {
        params = calloc((size_t)fn->func_def.params->count, sizeof(Type *));
        if (!params) {
            perror("calloc(params)");
            exit(1);
        }
    }

    for (int i = 0; i < fn->func_def.params->count; i++) {
        ASTNode *p = fn->func_def.params->items[i];
        Type *pt = type_from_ast(ctx, p->param.type);
        if (p->param.is_array) pt = make_sem_type(TY_ARRAY, pt, NULL, 0);
        params[i] = pt;
    }

    Type *ret = type_from_ast(ctx, fn->func_def.ret_type);
    Type *fn_type = make_function_type(ret, params, fn->func_def.params->count);
    scope_add(ctx->scope, fn->func_def.name, fn_type, fn->line);

    Type *prev_ret = ctx->current_ret_type;
    ctx->current_ret_type = ret;

    ctx->scope = scope_push(ctx->scope);
    for (int i = 0; i < fn->func_def.params->count; i++) {
        ASTNode *p = fn->func_def.params->items[i];
        if (scope_lookup_local(ctx->scope, p->param.name)) {
            sem_error(ctx, p->line, "Duplicate parameter name", p->param.name);
            continue;
        }
        scope_add(ctx->scope, p->param.name, params[i], p->line);
    }
    semantic_stmt(ctx, fn->func_def.body);
    ctx->scope = scope_pop(ctx->scope);
    ctx->current_ret_type = prev_ret;
}

static void semantic_struct_decl(SemCtx *ctx, ASTNode *decl) {
    if (find_struct(ctx, decl->struct_decl.name)) {
        sem_error(ctx, decl->line, "Redeclaration of struct", decl->struct_decl.name);
        return;
    }
    StructDef *sd = calloc(1, sizeof(StructDef));
    if (!sd) {
        perror("calloc(StructDef)");
        exit(1);
    }
    sd->name = decl->struct_decl.name;
    sd->members = scope_push(NULL);
    sd->next = ctx->structs;
    ctx->structs = sd;

    for (int i = 0; i < decl->struct_decl.members->count; i++) {
        ASTNode *m = decl->struct_decl.members->items[i];
        if (m->kind != NODE_VAR_DECL) continue;
        if (scope_lookup_local(sd->members, m->var_decl.name)) {
            sem_error(ctx, m->line, "Duplicate struct field", m->var_decl.name);
            continue;
        }
        Type *mt = type_from_ast(ctx, m->var_decl.type);
        if (m->var_decl.arr_size > 0) mt = make_sem_type(TY_ARRAY, mt, NULL, m->var_decl.arr_size);
        scope_add(sd->members, m->var_decl.name, mt, m->line);
    }
}

static void semantic_decl(SemCtx *ctx, ASTNode *decl) {
    if (!decl) return;
    switch (decl->kind) {
    case NODE_VAR_DECL: check_var_decl(ctx, decl); break;
    case NODE_ARRAY_DECL: check_array_decl(ctx, decl); break;
    case NODE_FUNC_DEF: semantic_function(ctx, decl); break;
    case NODE_STRUCT_DECL: semantic_struct_decl(ctx, decl); break;
    default: break;
    }
}

int semantic_analyze(ASTNode *root) {
    if (!root || root->kind != NODE_PROGRAM) return 1;

    SemCtx ctx = {0};
    ctx.scope = scope_push(NULL);
    ctx.current_ret_type = ty_void();
    scope_add(ctx.scope, "printf", make_variadic_function_type(ty_int()), 0);
    scope_add(ctx.scope, "scanf", make_variadic_function_type(ty_int()), 0);

    for (int i = 0; i < root->program.decls->count; i++) {
        semantic_decl(&ctx, root->program.decls->items[i]);
    }

    ctx.scope = scope_pop(ctx.scope);
    return ctx.errors == 0 ? 0 : 1;
}
