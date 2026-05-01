/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    KW_INT = 258,                  /* KW_INT  */
    KW_FLOAT = 259,                /* KW_FLOAT  */
    KW_CHAR = 260,                 /* KW_CHAR  */
    KW_BOOL = 261,                 /* KW_BOOL  */
    KW_VOID = 262,                 /* KW_VOID  */
    KW_IF = 263,                   /* KW_IF  */
    KW_ELSE = 264,                 /* KW_ELSE  */
    KW_WHILE = 265,                /* KW_WHILE  */
    KW_DO = 266,                   /* KW_DO  */
    KW_FOR = 267,                  /* KW_FOR  */
    KW_RETURN = 268,               /* KW_RETURN  */
    KW_STRUCT = 269,               /* KW_STRUCT  */
    KW_BREAK = 270,                /* KW_BREAK  */
    KW_CONTINUE = 271,             /* KW_CONTINUE  */
    KW_PRINTF = 272,               /* KW_PRINTF  */
    KW_SCANF = 273,                /* KW_SCANF  */
    INT_LITERAL = 274,             /* INT_LITERAL  */
    BOOL_LITERAL = 275,            /* BOOL_LITERAL  */
    FLOAT_LITERAL = 276,           /* FLOAT_LITERAL  */
    CHAR_LITERAL = 277,            /* CHAR_LITERAL  */
    STR_LITERAL = 278,             /* STR_LITERAL  */
    IDENTIFIER = 279,              /* IDENTIFIER  */
    OP_PLUS = 280,                 /* OP_PLUS  */
    OP_MINUS = 281,                /* OP_MINUS  */
    OP_MUL = 282,                  /* OP_MUL  */
    OP_DIV = 283,                  /* OP_DIV  */
    OP_MOD = 284,                  /* OP_MOD  */
    OP_ASSIGN = 285,               /* OP_ASSIGN  */
    OP_PLUS_ASSIGN = 286,          /* OP_PLUS_ASSIGN  */
    OP_MINUS_ASSIGN = 287,         /* OP_MINUS_ASSIGN  */
    OP_MUL_ASSIGN = 288,           /* OP_MUL_ASSIGN  */
    OP_DIV_ASSIGN = 289,           /* OP_DIV_ASSIGN  */
    OP_MOD_ASSIGN = 290,           /* OP_MOD_ASSIGN  */
    OP_AND_ASSIGN = 291,           /* OP_AND_ASSIGN  */
    OP_OR_ASSIGN = 292,            /* OP_OR_ASSIGN  */
    OP_XOR_ASSIGN = 293,           /* OP_XOR_ASSIGN  */
    OP_LSHIFT_ASSIGN = 294,        /* OP_LSHIFT_ASSIGN  */
    OP_RSHIFT_ASSIGN = 295,        /* OP_RSHIFT_ASSIGN  */
    OP_EQ = 296,                   /* OP_EQ  */
    OP_NEQ = 297,                  /* OP_NEQ  */
    OP_LT = 298,                   /* OP_LT  */
    OP_GT = 299,                   /* OP_GT  */
    OP_LEQ = 300,                  /* OP_LEQ  */
    OP_GEQ = 301,                  /* OP_GEQ  */
    OP_AND = 302,                  /* OP_AND  */
    OP_OR = 303,                   /* OP_OR  */
    OP_NOT = 304,                  /* OP_NOT  */
    OP_INC = 305,                  /* OP_INC  */
    OP_DEC = 306,                  /* OP_DEC  */
    OP_BITAND = 307,               /* OP_BITAND  */
    OP_BITOR = 308,                /* OP_BITOR  */
    OP_BITXOR = 309,               /* OP_BITXOR  */
    OP_BITNOT = 310,               /* OP_BITNOT  */
    OP_LSHIFT = 311,               /* OP_LSHIFT  */
    OP_RSHIFT = 312,               /* OP_RSHIFT  */
    OP_ARROW = 313,                /* OP_ARROW  */
    OP_DOT = 314,                  /* OP_DOT  */
    LPAREN = 315,                  /* LPAREN  */
    RPAREN = 316,                  /* RPAREN  */
    LBRACE = 317,                  /* LBRACE  */
    RBRACE = 318,                  /* RBRACE  */
    LBRACKET = 319,                /* LBRACKET  */
    RBRACKET = 320,                /* RBRACKET  */
    SEMICOLON = 321,               /* SEMICOLON  */
    COMMA = 322,                   /* COMMA  */
    COLON = 323,                   /* COLON  */
    QUESTION = 324,                /* QUESTION  */
    UNARY_PREC = 325               /* UNARY_PREC  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 30 "parser.y"

    int      ival;
    float    fval;
    char     cval;
    char    *sval;
    ASTNode *node;
    NodeList *list;

#line 143 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
