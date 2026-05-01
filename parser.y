%{
/*
 * parser.y — Bison Parser for MiniC Compiler
 * Member 1: Syntax Analysis + AST Construction
 *
 * Produces a full Abstract Syntax Tree (AST) for the source program.
 * The AST is then consumed by Member 2 (semantic analysis + TAC gen).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
extern int  yylex(void);
extern int  line_num;
extern int  col_num;
extern FILE *yyin;

/* ── root of the AST (exported to main.c) ───────────────────────────── */
ASTNode *ast_root = NULL;

/* ── error handler ───────────────────────────────────────────────────── */
void yyerror(const char *msg) {
    fprintf(stderr, "[Parse Error] Line %d, Col %d: %s\n",
            line_num, col_num, msg);
}
%}

/* ── Value union ─────────────────────────────────────────────────────── */
%union {
    int      ival;
    float    fval;
    char     cval;
    char    *sval;
    ASTNode *node;
    NodeList *list;
}

/* ── Token declarations ──────────────────────────────────────────────── */
/* keywords */
%token KW_INT KW_FLOAT KW_CHAR KW_BOOL KW_VOID
%token KW_IF KW_ELSE KW_WHILE KW_DO KW_FOR KW_RETURN
%token KW_STRUCT KW_BREAK KW_CONTINUE
%token KW_PRINTF KW_SCANF

/* literals */
%token <ival> INT_LITERAL BOOL_LITERAL
%token <fval> FLOAT_LITERAL
%token <cval> CHAR_LITERAL
%token <sval> STR_LITERAL
%token <sval> IDENTIFIER

/* operators */
%token OP_PLUS OP_MINUS OP_MUL OP_DIV OP_MOD
%token OP_ASSIGN
%token OP_PLUS_ASSIGN OP_MINUS_ASSIGN OP_MUL_ASSIGN OP_DIV_ASSIGN OP_MOD_ASSIGN
%token OP_AND_ASSIGN OP_OR_ASSIGN OP_XOR_ASSIGN OP_LSHIFT_ASSIGN OP_RSHIFT_ASSIGN
%token OP_EQ OP_NEQ OP_LT OP_GT OP_LEQ OP_GEQ
%token OP_AND OP_OR OP_NOT
%token OP_INC OP_DEC
%token OP_BITAND OP_BITOR OP_BITXOR OP_BITNOT OP_LSHIFT OP_RSHIFT
%token OP_ARROW OP_DOT

/* punctuation */
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMICOLON COMMA COLON QUESTION

/* ── Non-terminal types ──────────────────────────────────────────────── */
%type <node>  program
%type <list>  translation_unit declaration_list param_list_ne arg_list_ne
%type <node>  declaration function_def var_decl struct_decl
%type <node>  type_spec
%type <node>  compound_stmt
%type <list>  stmt_list
%type <node>  stmt
%type <node>  if_stmt while_stmt do_while_stmt for_stmt return_stmt break_stmt continue_stmt
%type <node>  expr_stmt
%type <node>  expr assign_expr ternary_expr or_expr and_expr
%type <node>  bitor_expr bitxor_expr bitand_expr
%type <node>  eq_expr rel_expr shift_expr add_expr mul_expr unary_expr
%type <node>  postfix_expr primary_expr
%type <list>  arg_list param_list
%type <node>  param
%type <node>  init_opt
%type <node>  for_init for_cond for_incr

/* ── Operator precedence (low → high) ────────────────────────────────── */
%right OP_ASSIGN OP_PLUS_ASSIGN OP_MINUS_ASSIGN OP_MUL_ASSIGN OP_DIV_ASSIGN OP_MOD_ASSIGN OP_AND_ASSIGN OP_OR_ASSIGN OP_XOR_ASSIGN OP_LSHIFT_ASSIGN OP_RSHIFT_ASSIGN
%right QUESTION COLON
%left  OP_OR
%left  OP_AND
%left  OP_BITOR
%left  OP_BITXOR
%left  OP_BITAND
%left  OP_EQ OP_NEQ
%left  OP_LT OP_GT OP_LEQ OP_GEQ
%left  OP_LSHIFT OP_RSHIFT
%left  OP_PLUS OP_MINUS
%left  OP_MUL OP_DIV OP_MOD
%right UNARY_PREC        /* fictitious token for unary precedence */
%left  OP_INC OP_DEC OP_DOT OP_ARROW LBRACKET LPAREN

%expect 1   /* one known S/R conflict: dangling else */

%%

/* ═══════════════════════════════════════════════════════════════════════
   Top level
   ═══════════════════════════════════════════════════════════════════════ */

program
    : translation_unit
        { ast_root = make_program($1); }
    ;

translation_unit
    : /* empty */
        { $$ = nl_new(); }
    | translation_unit declaration
        { $$ = nl_append($1, $2); }
    ;

declaration
    : function_def  { $$ = $1; }
    | var_decl      { $$ = $1; }
    | struct_decl   { $$ = $1; }
    ;

/* ═══════════════════════════════════════════════════════════════════════
   Types
   ═══════════════════════════════════════════════════════════════════════ */

type_spec
    : KW_INT               { $$ = make_type("int");   }
    | KW_FLOAT             { $$ = make_type("float"); }
    | KW_CHAR              { $$ = make_type("char");  }
    | KW_BOOL              { $$ = make_type("bool");  }
    | KW_VOID              { $$ = make_type("void");  }
    | KW_STRUCT IDENTIFIER { $$ = make_struct_type($2); }
    ;

/* ═══════════════════════════════════════════════════════════════════════
   Struct declaration
   ═══════════════════════════════════════════════════════════════════════ */

struct_decl
    : KW_STRUCT IDENTIFIER LBRACE declaration_list RBRACE SEMICOLON
        { $$ = make_struct_decl($2, $4); }
    ;

declaration_list
    : /* empty */
        { $$ = nl_new(); }
    | declaration_list var_decl
        { $$ = nl_append($1, $2); }
    ;

/* ═══════════════════════════════════════════════════════════════════════
   Variable declaration   (supports arrays and optional initialiser)
   ═══════════════════════════════════════════════════════════════════════ */

var_decl
    : type_spec IDENTIFIER SEMICOLON
        { $$ = make_var_decl($1, $2, NULL, 0); }
    | type_spec IDENTIFIER OP_ASSIGN init_opt SEMICOLON
        { $$ = make_var_decl($1, $2, $4,   0); }
    | type_spec IDENTIFIER LBRACKET INT_LITERAL RBRACKET SEMICOLON
        { $$ = make_var_decl($1, $2, NULL, $4); }
    | type_spec IDENTIFIER LBRACKET INT_LITERAL RBRACKET OP_ASSIGN LBRACE arg_list RBRACE SEMICOLON
        { $$ = make_array_decl($1, $2, $4, $8); }
    ;

init_opt
    : expr      { $$ = $1; }
    ;

/* ═══════════════════════════════════════════════════════════════════════
   Function definition
   ═══════════════════════════════════════════════════════════════════════ */

function_def
    : type_spec IDENTIFIER LPAREN param_list RPAREN compound_stmt
        { $$ = make_func_def($1, $2, $4, $6); }
    ;

param_list
    : /* empty */
        { $$ = nl_new(); }
    | param_list_ne
        { $$ = $1; }
    ;

param_list_ne
    : param
        { $$ = nl_single($1); }
    | param_list_ne COMMA param
        { $$ = nl_append($1, $3); }
    ;

param
    : type_spec IDENTIFIER
        { $$ = make_param($1, $2, 0); }
    | type_spec IDENTIFIER LBRACKET RBRACKET
        { $$ = make_param($1, $2, 1); }   /* array param */
    ;

/* ═══════════════════════════════════════════════════════════════════════
   Statements
   ═══════════════════════════════════════════════════════════════════════ */

compound_stmt
    : LBRACE stmt_list RBRACE
        { $$ = make_compound($2); }
    ;

stmt_list
    : /* empty */
        { $$ = nl_new(); }
    | stmt_list stmt
        { $$ = nl_append($1, $2); }
    ;

stmt
    : var_decl          { $$ = $1; }
    | if_stmt           { $$ = $1; }
    | while_stmt        { $$ = $1; }
    | do_while_stmt     { $$ = $1; }
    | for_stmt          { $$ = $1; }
    | return_stmt       { $$ = $1; }
    | break_stmt        { $$ = $1; }
    | continue_stmt     { $$ = $1; }
    | compound_stmt     { $$ = $1; }
    | expr_stmt         { $$ = $1; }
    | SEMICOLON         { $$ = make_empty_stmt(); }   /* null statement */
    ;

if_stmt
    : KW_IF LPAREN expr RPAREN stmt
        { $$ = make_if($3, $5, NULL); }
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt
        { $$ = make_if($3, $5, $7); }
    ;

while_stmt
    : KW_WHILE LPAREN expr RPAREN stmt
        { $$ = make_while($3, $5); }
    ;

do_while_stmt
    : KW_DO stmt KW_WHILE LPAREN expr RPAREN SEMICOLON
        { $$ = make_do_while($2, $5); }
    ;

for_stmt
    : KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_incr RPAREN stmt
        { $$ = make_for($3, $5, $7, $9); }
    ;

/* for-init can be a var_decl (without trailing ;) or an expression */
for_init
    : type_spec IDENTIFIER OP_ASSIGN expr
        { $$ = make_var_decl($1, $2, $4, 0); }
    | expr
        { $$ = $1; }
    | /* empty */
        { $$ = NULL; }
    ;

for_cond
    : expr        { $$ = $1;   }
    | /* empty */ { $$ = NULL; }
    ;

for_incr
    : expr        { $$ = $1;   }
    | /* empty */ { $$ = NULL; }
    ;

return_stmt
    : KW_RETURN expr SEMICOLON
        { $$ = make_return($2); }
    | KW_RETURN SEMICOLON
        { $$ = make_return(NULL); }
    ;

break_stmt
    : KW_BREAK SEMICOLON
        { $$ = make_break(); }
    ;

continue_stmt
    : KW_CONTINUE SEMICOLON
        { $$ = make_continue(); }
    ;

expr_stmt
    : expr SEMICOLON
        { $$ = make_expr_stmt($1); }
    ;

/* ═══════════════════════════════════════════════════════════════════════
   Expressions  (precedence handled by grammar layers)
   ═══════════════════════════════════════════════════════════════════════ */

expr
    : assign_expr   { $$ = $1; }
    ;

assign_expr
    : ternary_expr                              { $$ = $1; }
    | unary_expr OP_ASSIGN      assign_expr    { $$ = make_binop("=",   $1, $3); }
    | unary_expr OP_PLUS_ASSIGN assign_expr    { $$ = make_binop("+=",  $1, $3); }
    | unary_expr OP_MINUS_ASSIGN assign_expr   { $$ = make_binop("-=",  $1, $3); }
    | unary_expr OP_MUL_ASSIGN  assign_expr    { $$ = make_binop("*=",  $1, $3); }
    | unary_expr OP_DIV_ASSIGN  assign_expr    { $$ = make_binop("/=",  $1, $3); }
    | unary_expr OP_MOD_ASSIGN  assign_expr    { $$ = make_binop("%=",  $1, $3); }
    | unary_expr OP_AND_ASSIGN  assign_expr    { $$ = make_binop("&=",  $1, $3); }
    | unary_expr OP_OR_ASSIGN   assign_expr    { $$ = make_binop("|=",  $1, $3); }
    | unary_expr OP_XOR_ASSIGN  assign_expr    { $$ = make_binop("^=",  $1, $3); }
    | unary_expr OP_LSHIFT_ASSIGN assign_expr  { $$ = make_binop("<<=", $1, $3); }
    | unary_expr OP_RSHIFT_ASSIGN assign_expr  { $$ = make_binop(">>=", $1, $3); }
    ;

ternary_expr
    : or_expr                               { $$ = $1; }
    | or_expr QUESTION expr COLON ternary_expr
        { $$ = make_ternary($1, $3, $5); }
    ;

or_expr
    : and_expr                      { $$ = $1; }
    | or_expr OP_OR and_expr        { $$ = make_binop("||", $1, $3); }
    ;

and_expr
    : bitor_expr                    { $$ = $1; }
    | and_expr OP_AND bitor_expr    { $$ = make_binop("&&", $1, $3); }
    ;

bitor_expr
    : bitxor_expr                       { $$ = $1; }
    | bitor_expr OP_BITOR bitxor_expr   { $$ = make_binop("|", $1, $3); }
    ;

bitxor_expr
    : bitand_expr                       { $$ = $1; }
    | bitxor_expr OP_BITXOR bitand_expr { $$ = make_binop("^", $1, $3); }
    ;

bitand_expr
    : eq_expr                           { $$ = $1; }
    | bitand_expr OP_BITAND eq_expr     { $$ = make_binop("&", $1, $3); }
    ;

eq_expr
    : rel_expr                      { $$ = $1; }
    | eq_expr OP_EQ  rel_expr       { $$ = make_binop("==", $1, $3); }
    | eq_expr OP_NEQ rel_expr       { $$ = make_binop("!=", $1, $3); }
    ;

rel_expr
    : shift_expr                    { $$ = $1; }
    | rel_expr OP_LT  shift_expr    { $$ = make_binop("<",  $1, $3); }
    | rel_expr OP_GT  shift_expr    { $$ = make_binop(">",  $1, $3); }
    | rel_expr OP_LEQ shift_expr    { $$ = make_binop("<=", $1, $3); }
    | rel_expr OP_GEQ shift_expr    { $$ = make_binop(">=", $1, $3); }
    ;

shift_expr
    : add_expr                          { $$ = $1; }
    | shift_expr OP_LSHIFT add_expr     { $$ = make_binop("<<", $1, $3); }
    | shift_expr OP_RSHIFT add_expr     { $$ = make_binop(">>", $1, $3); }
    ;

add_expr
    : mul_expr                      { $$ = $1; }
    | add_expr OP_PLUS  mul_expr    { $$ = make_binop("+", $1, $3); }
    | add_expr OP_MINUS mul_expr    { $$ = make_binop("-", $1, $3); }
    ;

mul_expr
    : unary_expr                    { $$ = $1; }
    | mul_expr OP_MUL  unary_expr   { $$ = make_binop("*", $1, $3); }
    | mul_expr OP_DIV  unary_expr   { $$ = make_binop("/", $1, $3); }
    | mul_expr OP_MOD  unary_expr   { $$ = make_binop("%", $1, $3); }
    ;

unary_expr
    : postfix_expr                          { $$ = $1; }
    | OP_MINUS unary_expr %prec UNARY_PREC  { $$ = make_unop("neg", $2);   }
    | OP_NOT   unary_expr %prec UNARY_PREC  { $$ = make_unop("!",   $2);   }
    | OP_BITNOT unary_expr %prec UNARY_PREC { $$ = make_unop("~",   $2);   }
    | OP_INC   unary_expr %prec UNARY_PREC  { $$ = make_unop("pre++", $2); }
    | OP_DEC   unary_expr %prec UNARY_PREC  { $$ = make_unop("pre--", $2); }
    | OP_BITAND unary_expr %prec UNARY_PREC { $$ = make_unop("addr", $2);  }
    | OP_MUL    unary_expr %prec UNARY_PREC { $$ = make_unop("deref", $2); }
    | LPAREN type_spec RPAREN unary_expr %prec UNARY_PREC
        { $$ = make_cast($2, $4); }
    ;

postfix_expr
    : primary_expr                                  { $$ = $1; }
    | postfix_expr LBRACKET expr RBRACKET           { $$ = make_index($1, $3); }
    | postfix_expr LPAREN arg_list RPAREN           { $$ = make_call($1, $3); }
    | postfix_expr OP_DOT   IDENTIFIER              { $$ = make_member($1, $3, 0); }
    | postfix_expr OP_ARROW IDENTIFIER              { $$ = make_member($1, $3, 1); }
    | postfix_expr OP_INC                           { $$ = make_unop("post++", $1); }
    | postfix_expr OP_DEC                           { $$ = make_unop("post--", $1); }
    ;

primary_expr
    : INT_LITERAL       { $$ = make_int_lit($1);   }
    | FLOAT_LITERAL     { $$ = make_float_lit($1); }
    | CHAR_LITERAL      { $$ = make_char_lit($1);  }
    | STR_LITERAL       { $$ = make_str_lit($1);   }
    | BOOL_LITERAL      { $$ = make_bool_lit($1);  }
    | IDENTIFIER        { $$ = make_ident($1);     }
    | KW_PRINTF         { $$ = make_ident("printf"); }
    | KW_SCANF          { $$ = make_ident("scanf");  }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

/* ── Argument lists ───────────────────────────────────────────────────── */
arg_list
    : /* empty */   { $$ = nl_new(); }
    | arg_list_ne   { $$ = $1; }
    ;

arg_list_ne
    : expr
        { $$ = nl_single($1); }
    | arg_list_ne COMMA expr
        { $$ = nl_append($1, $3); }
    ;

%%
