/*
 * ast.h  —  Abstract Syntax Tree definitions for MiniC
 *
 * PURPOSE
 *   Defines every AST node kind, the ASTNode struct, the NodeList helper,
 *   and all constructor / printer / free prototypes.
 *
 * AUDIENCE
 *   Member 1  : produces this AST from the parser actions.
 *   Member 2  : walks this AST for semantic analysis and TAC generation.
 *   Member 3  : receives TAC; does not need ast.h directly.
 *
 * DESIGN NOTES
 *   - Every node carries a `line` field for error messages.
 *   - NodeList is a simple growable array used for parameter lists,
 *     argument lists, struct member lists, statement lists, etc.
 *   - Strings (names, operator symbols) are heap-allocated via strdup().
 *   - The tree is intentionally NOT reference-counted; call ast_free()
 *     exactly once on the root after you are done with the tree.
 */

#ifndef AST_H
#define AST_H

#include <stdio.h>

/* =========================================================================
   Node-kind enumeration
   ========================================================================= */
typedef enum {
    /* ---- Top level ---- */
    NODE_PROGRAM,

    /* ---- Declarations ---- */
    NODE_FUNC_DEF,       /* return-type  name ( params ) body          */
    NODE_VAR_DECL,       /* type  name [= init] [array-size]           */
    NODE_ARRAY_DECL,     /* type  name [ size ] = { init-list }        */
    NODE_STRUCT_DECL,    /* struct name { members }                    */
    NODE_PARAM,          /* type  name  [is_array]                     */

    /* ---- Type nodes ---- */
    NODE_TYPE,           /* "int" | "float" | "char" | "bool" | "void" */
    NODE_STRUCT_TYPE,    /* "struct Foo"                                */

    /* ---- Statements ---- */
    NODE_COMPOUND,       /* { stmt* }                                   */
    NODE_IF,             /* if (cond) then [else else_br]               */
    NODE_WHILE,          /* while (cond) body                           */
    NODE_DO_WHILE,       /* do body while (cond)                        */
    NODE_FOR,            /* for (init; cond; incr) body                 */
    NODE_RETURN,         /* return [expr]                               */
    NODE_BREAK,          /* break                                       */
    NODE_CONTINUE,       /* continue                                    */
    NODE_EXPR_STMT,      /* expr ;                                      */
    NODE_EMPTY_STMT,     /* ;                                           */

    /* ---- Expressions ---- */
    NODE_BINOP,          /* left OP right                               */
    NODE_UNOP,           /* OP operand  (or  operand OP for post-fix)  */
    NODE_TERNARY,        /* cond ? then : else                          */
    NODE_CAST,           /* (type) expr                                 */
    NODE_CALL,           /* callee ( args )                             */
    NODE_INDEX,          /* array [ index ]                             */
    NODE_MEMBER,         /* obj.field  or  obj->field                  */
    NODE_SIZEOF,         /* sizeof(type) or sizeof(expr)               */

    /* ---- Literals ---- */
    NODE_INT_LIT,
    NODE_FLOAT_LIT,
    NODE_CHAR_LIT,
    NODE_STR_LIT,
    NODE_BOOL_LIT,

    /* ---- Identifier ---- */
    NODE_IDENT,
} NodeKind;

/* =========================================================================
   Forward declarations
   ========================================================================= */
typedef struct ASTNode  ASTNode;
typedef struct NodeList NodeList;

/* =========================================================================
   NodeList  —  growable array of ASTNode pointers
   ========================================================================= */
struct NodeList {
    ASTNode **items;
    int       count;
    int       capacity;
};

/* =========================================================================
   ASTNode
   ========================================================================= */
struct ASTNode {
    NodeKind kind;
    int      line;    /* 1-based source line (for error messages)      */

    union {

        /* NODE_PROGRAM */
        struct { NodeList *decls; } program;

        /* NODE_FUNC_DEF */
        struct {
            ASTNode  *ret_type;
            char     *name;
            NodeList *params;
            ASTNode  *body;
        } func_def;

        /* NODE_VAR_DECL */
        struct {
            ASTNode *type;
            char    *name;
            ASTNode *init;       /* NULL  →  no initialiser              */
            int      arr_size;   /* 0     →  not an array                */
        } var_decl;

        /* NODE_ARRAY_DECL (with initialiser list) */
        struct {
            ASTNode  *type;
            char     *name;
            int       size;
            NodeList *init_list;
        } arr_decl;

        /* NODE_STRUCT_DECL */
        struct {
            char     *name;
            NodeList *members;   /* list of NODE_VAR_DECL                */
        } struct_decl;

        /* NODE_PARAM */
        struct {
            ASTNode *type;
            char    *name;
            int      is_array;   /* 1 → declared as  type name[]         */
        } param;

        /* NODE_TYPE */
        struct { char *name; } type;

        /* NODE_STRUCT_TYPE */
        struct { char *name; } struct_type;

        /* NODE_COMPOUND */
        struct { NodeList *stmts; } compound;

        /* NODE_IF */
        struct {
            ASTNode *cond;
            ASTNode *then_br;
            ASTNode *else_br;    /* NULL  →  no else                     */
        } if_stmt;

        /* NODE_WHILE */
        struct {
            ASTNode *cond;
            ASTNode *body;
        } while_stmt;

        /* NODE_DO_WHILE */
        struct {
            ASTNode *body;
            ASTNode *cond;
        } do_while_stmt;

        /* NODE_FOR */
        struct {
            ASTNode *init;       /* may be NULL                          */
            ASTNode *cond;       /* may be NULL (treated as true)        */
            ASTNode *incr;       /* may be NULL                          */
            ASTNode *body;
        } for_stmt;

        /* NODE_RETURN */
        struct { ASTNode *value; } ret_stmt;   /* value may be NULL     */

        /* NODE_EXPR_STMT */
        struct { ASTNode *expr; } expr_stmt;

        /* NODE_BINOP */
        struct {
            char    *op;         /* e.g. "+", "<=", "&&", "="           */
            ASTNode *left;
            ASTNode *right;
        } binop;

        /* NODE_UNOP */
        struct {
            char    *op;         /* "neg","!","~","pre++","pre--",
                                    "post++","post--","addr","deref"    */
            ASTNode *operand;
        } unop;

        /* NODE_TERNARY */
        struct {
            ASTNode *cond;
            ASTNode *then_expr;
            ASTNode *else_expr;
        } ternary;

        /* NODE_CAST */
        struct {
            ASTNode *type;
            ASTNode *expr;
        } cast;

        /* NODE_CALL */
        struct {
            ASTNode  *callee;    /* usually NODE_IDENT                   */
            NodeList *args;
        } call;

        /* NODE_INDEX */
        struct {
            ASTNode *array;
            ASTNode *index;
        } index;

        /* NODE_MEMBER */
        struct {
            ASTNode *object;
            char    *field;
            int      arrow;      /* 1 → ->,  0 → .                       */
        } member;

        /* NODE_SIZEOF */
        struct {
            ASTNode *type_or_expr;   /* NODE_TYPE or any expression      */
            int      is_type;        /* 1 → sizeof(type), 0 → sizeof(expr)*/
        } sizeof_expr;

        /* Literals */
        struct { int   ival; } int_lit;
        struct { double fval; } float_lit;
        struct { char  cval; } char_lit;
        struct { char *sval; } str_lit;
        struct { int   bval; } bool_lit;

        /* NODE_IDENT */
        struct { char *name; } ident;
    };
};

/* =========================================================================
   NodeList API
   ========================================================================= */
NodeList *nl_new    (void);
NodeList *nl_single (ASTNode *node);
NodeList *nl_append (NodeList *list, ASTNode *node);

/* =========================================================================
   ASTNode constructors
   ========================================================================= */
ASTNode *make_program     (NodeList *decls);

ASTNode *make_func_def    (ASTNode *ret, char *name,
                            NodeList *params, ASTNode *body);
ASTNode *make_var_decl    (ASTNode *type, char *name,
                            ASTNode *init, int arr_size);
ASTNode *make_array_decl  (ASTNode *type, char *name,
                            int size, NodeList *inits);
ASTNode *make_struct_decl (char *name, NodeList *members);
ASTNode *make_param       (ASTNode *type, char *name, int is_array);

ASTNode *make_type        (const char *name);
ASTNode *make_struct_type (char *name);

ASTNode *make_compound    (NodeList *stmts);
ASTNode *make_if          (ASTNode *cond, ASTNode *then_br, ASTNode *else_br);
ASTNode *make_while       (ASTNode *cond, ASTNode *body);
ASTNode *make_do_while    (ASTNode *body, ASTNode *cond);
ASTNode *make_for         (ASTNode *init, ASTNode *cond,
                            ASTNode *incr, ASTNode *body);
ASTNode *make_return      (ASTNode *val);
ASTNode *make_break       (void);
ASTNode *make_continue    (void);
ASTNode *make_empty_stmt  (void);
ASTNode *make_expr_stmt   (ASTNode *expr);

ASTNode *make_binop       (const char *op, ASTNode *l, ASTNode *r);
ASTNode *make_unop        (const char *op, ASTNode *operand);
ASTNode *make_ternary     (ASTNode *cond, ASTNode *th, ASTNode *el);
ASTNode *make_cast        (ASTNode *type, ASTNode *expr);
ASTNode *make_call        (ASTNode *callee, NodeList *args);
ASTNode *make_index       (ASTNode *arr, ASTNode *idx);
ASTNode *make_member      (ASTNode *obj, char *field, int arrow);
ASTNode *make_sizeof_type (ASTNode *type);
ASTNode *make_sizeof_expr (ASTNode *expr);

ASTNode *make_int_lit     (int v);
ASTNode *make_float_lit   (double v);
ASTNode *make_char_lit    (char v);
ASTNode *make_str_lit     (const char *v);
ASTNode *make_bool_lit    (int v);
ASTNode *make_ident       (const char *name);

/* =========================================================================
   Utilities
   ========================================================================= */
void ast_print (ASTNode *node, int indent);   /* pretty-print to stdout */
void ast_free  (ASTNode *node);               /* recursively free       */

/* Exported from parser */
extern ASTNode *ast_root;

#endif /* AST_H */
