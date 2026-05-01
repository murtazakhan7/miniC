/*
 * ast.c  —  AST node constructors, NodeList helpers, printer, and free.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* line_num is updated by the lexer */
extern int line_num;

/* =========================================================================
   Internal allocator
   ========================================================================= */
static ASTNode *alloc_node(NodeKind kind)
{
    ASTNode *n = calloc(1, sizeof(ASTNode));
    if (!n) { perror("calloc(ASTNode)"); exit(1); }
    n->kind = kind;
    n->line = line_num;
    return n;
}

/* =========================================================================
   NodeList
   ========================================================================= */
NodeList *nl_new(void)
{
    NodeList *l = calloc(1, sizeof(NodeList));
    if (!l) { perror("calloc(NodeList)"); exit(1); }
    l->capacity = 8;
    l->items    = malloc(l->capacity * sizeof(ASTNode *));
    if (!l->items) { perror("malloc"); exit(1); }
    l->count    = 0;
    return l;
}

NodeList *nl_single(ASTNode *node)
{
    NodeList *l = nl_new();
    l->items[0] = node;
    l->count    = 1;
    return l;
}

NodeList *nl_append(NodeList *list, ASTNode *node)
{
    if (list->count == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items,
                              list->capacity * sizeof(ASTNode *));
        if (!list->items) { perror("realloc"); exit(1); }
    }
    list->items[list->count++] = node;
    return list;
}

/* =========================================================================
   Constructors
   ========================================================================= */

ASTNode *make_program(NodeList *decls)
{
    ASTNode *n = alloc_node(NODE_PROGRAM);
    n->program.decls = decls;
    return n;
}

ASTNode *make_func_def(ASTNode *ret, char *name,
                        NodeList *params, ASTNode *body)
{
    ASTNode *n = alloc_node(NODE_FUNC_DEF);
    n->func_def.ret_type = ret;
    n->func_def.name     = name;          /* already strdup'd by lexer */
    n->func_def.params   = params;
    n->func_def.body     = body;
    return n;
}

ASTNode *make_var_decl(ASTNode *type, char *name,
                        ASTNode *init, int arr_size)
{
    ASTNode *n = alloc_node(NODE_VAR_DECL);
    n->var_decl.type     = type;
    n->var_decl.name     = name;
    n->var_decl.init     = init;
    n->var_decl.arr_size = arr_size;
    return n;
}

ASTNode *make_array_decl(ASTNode *type, char *name,
                          int size, NodeList *inits)
{
    ASTNode *n = alloc_node(NODE_ARRAY_DECL);
    n->arr_decl.type      = type;
    n->arr_decl.name      = name;
    n->arr_decl.size      = size;
    n->arr_decl.init_list = inits;
    return n;
}

ASTNode *make_struct_decl(char *name, NodeList *members)
{
    ASTNode *n = alloc_node(NODE_STRUCT_DECL);
    n->struct_decl.name    = name;
    n->struct_decl.members = members;
    return n;
}

ASTNode *make_param(ASTNode *type, char *name, int is_array)
{
    ASTNode *n = alloc_node(NODE_PARAM);
    n->param.type     = type;
    n->param.name     = name;
    n->param.is_array = is_array;
    return n;
}

ASTNode *make_type(const char *name)
{
    ASTNode *n = alloc_node(NODE_TYPE);
    n->type.name = strdup(name);
    return n;
}

ASTNode *make_struct_type(char *name)
{
    ASTNode *n = alloc_node(NODE_STRUCT_TYPE);
    n->struct_type.name = name;
    return n;
}

ASTNode *make_compound(NodeList *stmts)
{
    ASTNode *n = alloc_node(NODE_COMPOUND);
    n->compound.stmts = stmts;
    return n;
}

ASTNode *make_if(ASTNode *cond, ASTNode *then_br, ASTNode *else_br)
{
    ASTNode *n = alloc_node(NODE_IF);
    n->if_stmt.cond    = cond;
    n->if_stmt.then_br = then_br;
    n->if_stmt.else_br = else_br;
    return n;
}

ASTNode *make_while(ASTNode *cond, ASTNode *body)
{
    ASTNode *n = alloc_node(NODE_WHILE);
    n->while_stmt.cond = cond;
    n->while_stmt.body = body;
    return n;
}

ASTNode *make_do_while(ASTNode *body, ASTNode *cond)
{
    ASTNode *n = alloc_node(NODE_DO_WHILE);
    n->do_while_stmt.body = body;
    n->do_while_stmt.cond = cond;
    return n;
}

ASTNode *make_for(ASTNode *init, ASTNode *cond,
                   ASTNode *incr, ASTNode *body)
{
    ASTNode *n = alloc_node(NODE_FOR);
    n->for_stmt.init = init;
    n->for_stmt.cond = cond;
    n->for_stmt.incr = incr;
    n->for_stmt.body = body;
    return n;
}

ASTNode *make_return(ASTNode *val)
{
    ASTNode *n = alloc_node(NODE_RETURN);
    n->ret_stmt.value = val;
    return n;
}

ASTNode *make_break(void)    { return alloc_node(NODE_BREAK);    }
ASTNode *make_continue(void) { return alloc_node(NODE_CONTINUE); }
ASTNode *make_empty_stmt(void){ return alloc_node(NODE_EMPTY_STMT); }

ASTNode *make_expr_stmt(ASTNode *expr)
{
    ASTNode *n = alloc_node(NODE_EXPR_STMT);
    n->expr_stmt.expr = expr;
    return n;
}

ASTNode *make_binop(const char *op, ASTNode *l, ASTNode *r)
{
    ASTNode *n = alloc_node(NODE_BINOP);
    n->binop.op    = strdup(op);
    n->binop.left  = l;
    n->binop.right = r;
    return n;
}

ASTNode *make_unop(const char *op, ASTNode *operand)
{
    ASTNode *n = alloc_node(NODE_UNOP);
    n->unop.op      = strdup(op);
    n->unop.operand = operand;
    return n;
}

ASTNode *make_ternary(ASTNode *cond, ASTNode *th, ASTNode *el)
{
    ASTNode *n = alloc_node(NODE_TERNARY);
    n->ternary.cond      = cond;
    n->ternary.then_expr = th;
    n->ternary.else_expr = el;
    return n;
}

ASTNode *make_cast(ASTNode *type, ASTNode *expr)
{
    ASTNode *n = alloc_node(NODE_CAST);
    n->cast.type = type;
    n->cast.expr = expr;
    return n;
}

ASTNode *make_call(ASTNode *callee, NodeList *args)
{
    ASTNode *n = alloc_node(NODE_CALL);
    n->call.callee = callee;
    n->call.args   = args;
    return n;
}

ASTNode *make_index(ASTNode *arr, ASTNode *idx)
{
    ASTNode *n = alloc_node(NODE_INDEX);
    n->index.array = arr;
    n->index.index = idx;
    return n;
}

ASTNode *make_member(ASTNode *obj, char *field, int arrow)
{
    ASTNode *n = alloc_node(NODE_MEMBER);
    n->member.object = obj;
    n->member.field  = strdup(field);
    n->member.arrow  = arrow;
    return n;
}

ASTNode *make_sizeof_type(ASTNode *type)
{
    ASTNode *n = alloc_node(NODE_SIZEOF);
    n->sizeof_expr.type_or_expr = type;
    n->sizeof_expr.is_type      = 1;
    return n;
}

ASTNode *make_sizeof_expr(ASTNode *expr)
{
    ASTNode *n = alloc_node(NODE_SIZEOF);
    n->sizeof_expr.type_or_expr = expr;
    n->sizeof_expr.is_type      = 0;
    return n;
}

ASTNode *make_int_lit(int v)
{
    ASTNode *n = alloc_node(NODE_INT_LIT);
    n->int_lit.ival = v;
    return n;
}

ASTNode *make_float_lit(double v)
{
    ASTNode *n = alloc_node(NODE_FLOAT_LIT);
    n->float_lit.fval = v;
    return n;
}

ASTNode *make_char_lit(char v)
{
    ASTNode *n = alloc_node(NODE_CHAR_LIT);
    n->char_lit.cval = v;
    return n;
}

ASTNode *make_str_lit(const char *v)
{
    ASTNode *n = alloc_node(NODE_STR_LIT);
    n->str_lit.sval = strdup(v);
    return n;
}

ASTNode *make_bool_lit(int v)
{
    ASTNode *n = alloc_node(NODE_BOOL_LIT);
    n->bool_lit.bval = v;
    return n;
}

ASTNode *make_ident(const char *name)
{
    ASTNode *n = alloc_node(NODE_IDENT);
    n->ident.name = strdup(name);
    return n;
}

/* =========================================================================
   Pretty-printer
   ========================================================================= */
static void pad(int d) { for (int i=0;i<d*2;i++) putchar(' '); }

void ast_print(ASTNode *node, int d)
{
    if (!node) { pad(d); puts("<null>"); return; }

    switch (node->kind) {

    case NODE_PROGRAM:
        printf("Program  (%d top-level declarations)\n",
               node->program.decls->count);
        for (int i=0;i<node->program.decls->count;i++)
            ast_print(node->program.decls->items[i], d+1);
        break;

    case NODE_FUNC_DEF:
        pad(d); printf("FuncDef [line %d]: %s\n", node->line,
                       node->func_def.name);
        pad(d+1); printf("RetType: ");
        ast_print(node->func_def.ret_type, 0);
        pad(d+1); printf("Params (%d):\n",
                         node->func_def.params->count);
        for (int i=0;i<node->func_def.params->count;i++)
            ast_print(node->func_def.params->items[i], d+2);
        pad(d+1); printf("Body:\n");
        ast_print(node->func_def.body, d+2);
        break;

    case NODE_VAR_DECL:
        pad(d);
        printf("VarDecl [line %d]: %s", node->line,
               node->var_decl.name);
        if (node->var_decl.arr_size)
            printf("[%d]", node->var_decl.arr_size);
        putchar('\n');
        pad(d+1); printf("Type: ");
        ast_print(node->var_decl.type, 0);
        if (node->var_decl.init) {
            pad(d+1); printf("Init:\n");
            ast_print(node->var_decl.init, d+2);
        }
        break;

    case NODE_ARRAY_DECL:
        pad(d); printf("ArrayDecl [line %d]: %s[%d]\n",
                        node->line,
                        node->arr_decl.name,
                        node->arr_decl.size);
        pad(d+1); printf("Type: ");
        ast_print(node->arr_decl.type, 0);
        pad(d+1); printf("Initializers (%d):\n",
                         node->arr_decl.init_list->count);
        for (int i=0;i<node->arr_decl.init_list->count;i++)
            ast_print(node->arr_decl.init_list->items[i], d+2);
        break;

    case NODE_STRUCT_DECL:
        pad(d); printf("StructDecl [line %d]: %s (%d members)\n",
                        node->line, node->struct_decl.name,
                        node->struct_decl.members->count);
        for (int i=0;i<node->struct_decl.members->count;i++)
            ast_print(node->struct_decl.members->items[i], d+1);
        break;

    case NODE_PARAM:
        pad(d); printf("Param: %s%s — ",
                        node->param.name,
                        node->param.is_array ? "[]" : "");
        ast_print(node->param.type, 0);
        break;

    case NODE_TYPE:
        if (d==0) printf("Type(%s)\n", node->type.name);
        else { pad(d); printf("Type(%s)\n", node->type.name); }
        break;

    case NODE_STRUCT_TYPE:
        if (d==0) printf("struct %s\n", node->struct_type.name);
        else { pad(d); printf("struct %s\n", node->struct_type.name); }
        break;

    case NODE_COMPOUND:
        pad(d); printf("Compound (%d stmts)\n",
                        node->compound.stmts->count);
        for (int i=0;i<node->compound.stmts->count;i++)
            ast_print(node->compound.stmts->items[i], d+1);
        break;

    case NODE_IF:
        pad(d); printf("If\n");
        pad(d+1); printf("Cond:\n");
        ast_print(node->if_stmt.cond, d+2);
        pad(d+1); printf("Then:\n");
        ast_print(node->if_stmt.then_br, d+2);
        if (node->if_stmt.else_br) {
            pad(d+1); printf("Else:\n");
            ast_print(node->if_stmt.else_br, d+2);
        }
        break;

    case NODE_WHILE:
        pad(d); printf("While\n");
        pad(d+1); printf("Cond:\n"); ast_print(node->while_stmt.cond,d+2);
        pad(d+1); printf("Body:\n"); ast_print(node->while_stmt.body,d+2);
        break;

    case NODE_DO_WHILE:
        pad(d); printf("DoWhile\n");
        pad(d+1); printf("Body:\n"); ast_print(node->do_while_stmt.body,d+2);
        pad(d+1); printf("Cond:\n"); ast_print(node->do_while_stmt.cond,d+2);
        break;

    case NODE_FOR:
        pad(d); printf("For\n");
        if (node->for_stmt.init){pad(d+1);printf("Init:\n");ast_print(node->for_stmt.init,d+2);}
        if (node->for_stmt.cond){pad(d+1);printf("Cond:\n");ast_print(node->for_stmt.cond,d+2);}
        if (node->for_stmt.incr){pad(d+1);printf("Incr:\n");ast_print(node->for_stmt.incr,d+2);}
        pad(d+1); printf("Body:\n"); ast_print(node->for_stmt.body,d+2);
        break;

    case NODE_RETURN:
        pad(d); printf("Return\n");
        if (node->ret_stmt.value) ast_print(node->ret_stmt.value, d+1);
        break;

    case NODE_BREAK:     pad(d); puts("Break");     break;
    case NODE_CONTINUE:  pad(d); puts("Continue");  break;
    case NODE_EMPTY_STMT:pad(d); puts("EmptyStmt"); break;

    case NODE_EXPR_STMT:
        pad(d); printf("ExprStmt\n");
        ast_print(node->expr_stmt.expr, d+1);
        break;

    case NODE_BINOP:
        pad(d); printf("BinOp '%s'\n", node->binop.op);
        ast_print(node->binop.left,  d+1);
        ast_print(node->binop.right, d+1);
        break;

    case NODE_UNOP:
        pad(d); printf("UnOp '%s'\n", node->unop.op);
        ast_print(node->unop.operand, d+1);
        break;

    case NODE_TERNARY:
        pad(d); printf("Ternary\n");
        ast_print(node->ternary.cond,      d+1);
        ast_print(node->ternary.then_expr, d+1);
        ast_print(node->ternary.else_expr, d+1);
        break;

    case NODE_CAST:
        pad(d); printf("Cast\n");
        pad(d+1); printf("ToType: "); ast_print(node->cast.type, 0);
        ast_print(node->cast.expr, d+1);
        break;

    case NODE_CALL:
        pad(d); printf("Call (%d args)\n", node->call.args->count);
        pad(d+1); printf("Callee:\n");
        ast_print(node->call.callee, d+2);
        if (node->call.args->count) {
            pad(d+1); printf("Args:\n");
            for (int i=0;i<node->call.args->count;i++)
                ast_print(node->call.args->items[i], d+2);
        }
        break;

    case NODE_INDEX:
        pad(d); printf("Index\n");
        ast_print(node->index.array, d+1);
        ast_print(node->index.index, d+1);
        break;

    case NODE_MEMBER:
        pad(d); printf("Member %s '%s'\n",
                        node->member.arrow ? "->" : ".",
                        node->member.field);
        ast_print(node->member.object, d+1);
        break;

    case NODE_SIZEOF:
        pad(d); printf("Sizeof (%s)\n",
                        node->sizeof_expr.is_type ? "type" : "expr");
        ast_print(node->sizeof_expr.type_or_expr, d+1);
        break;

    case NODE_INT_LIT:
        pad(d); printf("IntLit(%d)\n",  node->int_lit.ival);   break;
    case NODE_FLOAT_LIT:
        pad(d); printf("FloatLit(%g)\n",node->float_lit.fval); break;
    case NODE_CHAR_LIT:
        pad(d); printf("CharLit('%c')\n",node->char_lit.cval); break;
    case NODE_STR_LIT:
        pad(d); printf("StrLit(%s)\n",  node->str_lit.sval);   break;
    case NODE_BOOL_LIT:
        pad(d); printf("BoolLit(%s)\n",
                        node->bool_lit.bval?"true":"false");    break;
    case NODE_IDENT:
        pad(d); printf("Ident(%s)\n",   node->ident.name);     break;

    default:
        pad(d); printf("<unknown node kind=%d>\n", node->kind);
    }
}

/* =========================================================================
   Free
   ========================================================================= */
static void nl_free(NodeList *l)
{
    if (!l) return;
    for (int i=0;i<l->count;i++) ast_free(l->items[i]);
    free(l->items);
    free(l);
}

void ast_free(ASTNode *n)
{
    if (!n) return;
    switch (n->kind) {
    case NODE_PROGRAM:     nl_free(n->program.decls); break;
    case NODE_FUNC_DEF:
        ast_free(n->func_def.ret_type);
        nl_free(n->func_def.params);
        ast_free(n->func_def.body);
        break;
    case NODE_VAR_DECL:
        ast_free(n->var_decl.type);
        ast_free(n->var_decl.init);
        break;
    case NODE_ARRAY_DECL:
        ast_free(n->arr_decl.type);
        nl_free(n->arr_decl.init_list);
        break;
    case NODE_STRUCT_DECL: nl_free(n->struct_decl.members); break;
    case NODE_PARAM:       ast_free(n->param.type);          break;
    case NODE_COMPOUND:    nl_free(n->compound.stmts);       break;
    case NODE_IF:
        ast_free(n->if_stmt.cond);
        ast_free(n->if_stmt.then_br);
        ast_free(n->if_stmt.else_br);
        break;
    case NODE_WHILE:
        ast_free(n->while_stmt.cond);
        ast_free(n->while_stmt.body);
        break;
    case NODE_DO_WHILE:
        ast_free(n->do_while_stmt.body);
        ast_free(n->do_while_stmt.cond);
        break;
    case NODE_FOR:
        ast_free(n->for_stmt.init);
        ast_free(n->for_stmt.cond);
        ast_free(n->for_stmt.incr);
        ast_free(n->for_stmt.body);
        break;
    case NODE_RETURN:   ast_free(n->ret_stmt.value); break;
    case NODE_EXPR_STMT:ast_free(n->expr_stmt.expr); break;
    case NODE_BINOP:
        ast_free(n->binop.left);
        ast_free(n->binop.right);
        break;
    case NODE_UNOP:     ast_free(n->unop.operand); break;
    case NODE_TERNARY:
        ast_free(n->ternary.cond);
        ast_free(n->ternary.then_expr);
        ast_free(n->ternary.else_expr);
        break;
    case NODE_CAST:
        ast_free(n->cast.type);
        ast_free(n->cast.expr);
        break;
    case NODE_CALL:
        ast_free(n->call.callee);
        nl_free(n->call.args);
        break;
    case NODE_INDEX:
        ast_free(n->index.array);
        ast_free(n->index.index);
        break;
    case NODE_MEMBER:   ast_free(n->member.object); break;
    case NODE_SIZEOF:   ast_free(n->sizeof_expr.type_or_expr); break;
    default: break;
    }
    free(n);
}
