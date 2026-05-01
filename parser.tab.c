/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

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

#line 99 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_KW_INT = 3,                     /* KW_INT  */
  YYSYMBOL_KW_FLOAT = 4,                   /* KW_FLOAT  */
  YYSYMBOL_KW_CHAR = 5,                    /* KW_CHAR  */
  YYSYMBOL_KW_BOOL = 6,                    /* KW_BOOL  */
  YYSYMBOL_KW_VOID = 7,                    /* KW_VOID  */
  YYSYMBOL_KW_IF = 8,                      /* KW_IF  */
  YYSYMBOL_KW_ELSE = 9,                    /* KW_ELSE  */
  YYSYMBOL_KW_WHILE = 10,                  /* KW_WHILE  */
  YYSYMBOL_KW_DO = 11,                     /* KW_DO  */
  YYSYMBOL_KW_FOR = 12,                    /* KW_FOR  */
  YYSYMBOL_KW_RETURN = 13,                 /* KW_RETURN  */
  YYSYMBOL_KW_STRUCT = 14,                 /* KW_STRUCT  */
  YYSYMBOL_KW_BREAK = 15,                  /* KW_BREAK  */
  YYSYMBOL_KW_CONTINUE = 16,               /* KW_CONTINUE  */
  YYSYMBOL_KW_PRINTF = 17,                 /* KW_PRINTF  */
  YYSYMBOL_KW_SCANF = 18,                  /* KW_SCANF  */
  YYSYMBOL_INT_LITERAL = 19,               /* INT_LITERAL  */
  YYSYMBOL_BOOL_LITERAL = 20,              /* BOOL_LITERAL  */
  YYSYMBOL_FLOAT_LITERAL = 21,             /* FLOAT_LITERAL  */
  YYSYMBOL_CHAR_LITERAL = 22,              /* CHAR_LITERAL  */
  YYSYMBOL_STR_LITERAL = 23,               /* STR_LITERAL  */
  YYSYMBOL_IDENTIFIER = 24,                /* IDENTIFIER  */
  YYSYMBOL_OP_PLUS = 25,                   /* OP_PLUS  */
  YYSYMBOL_OP_MINUS = 26,                  /* OP_MINUS  */
  YYSYMBOL_OP_MUL = 27,                    /* OP_MUL  */
  YYSYMBOL_OP_DIV = 28,                    /* OP_DIV  */
  YYSYMBOL_OP_MOD = 29,                    /* OP_MOD  */
  YYSYMBOL_OP_ASSIGN = 30,                 /* OP_ASSIGN  */
  YYSYMBOL_OP_PLUS_ASSIGN = 31,            /* OP_PLUS_ASSIGN  */
  YYSYMBOL_OP_MINUS_ASSIGN = 32,           /* OP_MINUS_ASSIGN  */
  YYSYMBOL_OP_MUL_ASSIGN = 33,             /* OP_MUL_ASSIGN  */
  YYSYMBOL_OP_DIV_ASSIGN = 34,             /* OP_DIV_ASSIGN  */
  YYSYMBOL_OP_MOD_ASSIGN = 35,             /* OP_MOD_ASSIGN  */
  YYSYMBOL_OP_AND_ASSIGN = 36,             /* OP_AND_ASSIGN  */
  YYSYMBOL_OP_OR_ASSIGN = 37,              /* OP_OR_ASSIGN  */
  YYSYMBOL_OP_XOR_ASSIGN = 38,             /* OP_XOR_ASSIGN  */
  YYSYMBOL_OP_LSHIFT_ASSIGN = 39,          /* OP_LSHIFT_ASSIGN  */
  YYSYMBOL_OP_RSHIFT_ASSIGN = 40,          /* OP_RSHIFT_ASSIGN  */
  YYSYMBOL_OP_EQ = 41,                     /* OP_EQ  */
  YYSYMBOL_OP_NEQ = 42,                    /* OP_NEQ  */
  YYSYMBOL_OP_LT = 43,                     /* OP_LT  */
  YYSYMBOL_OP_GT = 44,                     /* OP_GT  */
  YYSYMBOL_OP_LEQ = 45,                    /* OP_LEQ  */
  YYSYMBOL_OP_GEQ = 46,                    /* OP_GEQ  */
  YYSYMBOL_OP_AND = 47,                    /* OP_AND  */
  YYSYMBOL_OP_OR = 48,                     /* OP_OR  */
  YYSYMBOL_OP_NOT = 49,                    /* OP_NOT  */
  YYSYMBOL_OP_INC = 50,                    /* OP_INC  */
  YYSYMBOL_OP_DEC = 51,                    /* OP_DEC  */
  YYSYMBOL_OP_BITAND = 52,                 /* OP_BITAND  */
  YYSYMBOL_OP_BITOR = 53,                  /* OP_BITOR  */
  YYSYMBOL_OP_BITXOR = 54,                 /* OP_BITXOR  */
  YYSYMBOL_OP_BITNOT = 55,                 /* OP_BITNOT  */
  YYSYMBOL_OP_LSHIFT = 56,                 /* OP_LSHIFT  */
  YYSYMBOL_OP_RSHIFT = 57,                 /* OP_RSHIFT  */
  YYSYMBOL_OP_ARROW = 58,                  /* OP_ARROW  */
  YYSYMBOL_OP_DOT = 59,                    /* OP_DOT  */
  YYSYMBOL_LPAREN = 60,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 61,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 62,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 63,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 64,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 65,                  /* RBRACKET  */
  YYSYMBOL_SEMICOLON = 66,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 67,                     /* COMMA  */
  YYSYMBOL_COLON = 68,                     /* COLON  */
  YYSYMBOL_QUESTION = 69,                  /* QUESTION  */
  YYSYMBOL_UNARY_PREC = 70,                /* UNARY_PREC  */
  YYSYMBOL_YYACCEPT = 71,                  /* $accept  */
  YYSYMBOL_program = 72,                   /* program  */
  YYSYMBOL_translation_unit = 73,          /* translation_unit  */
  YYSYMBOL_declaration = 74,               /* declaration  */
  YYSYMBOL_type_spec = 75,                 /* type_spec  */
  YYSYMBOL_struct_decl = 76,               /* struct_decl  */
  YYSYMBOL_declaration_list = 77,          /* declaration_list  */
  YYSYMBOL_var_decl = 78,                  /* var_decl  */
  YYSYMBOL_init_opt = 79,                  /* init_opt  */
  YYSYMBOL_function_def = 80,              /* function_def  */
  YYSYMBOL_param_list = 81,                /* param_list  */
  YYSYMBOL_param_list_ne = 82,             /* param_list_ne  */
  YYSYMBOL_param = 83,                     /* param  */
  YYSYMBOL_compound_stmt = 84,             /* compound_stmt  */
  YYSYMBOL_stmt_list = 85,                 /* stmt_list  */
  YYSYMBOL_stmt = 86,                      /* stmt  */
  YYSYMBOL_if_stmt = 87,                   /* if_stmt  */
  YYSYMBOL_while_stmt = 88,                /* while_stmt  */
  YYSYMBOL_do_while_stmt = 89,             /* do_while_stmt  */
  YYSYMBOL_for_stmt = 90,                  /* for_stmt  */
  YYSYMBOL_for_init = 91,                  /* for_init  */
  YYSYMBOL_for_cond = 92,                  /* for_cond  */
  YYSYMBOL_for_incr = 93,                  /* for_incr  */
  YYSYMBOL_return_stmt = 94,               /* return_stmt  */
  YYSYMBOL_break_stmt = 95,                /* break_stmt  */
  YYSYMBOL_continue_stmt = 96,             /* continue_stmt  */
  YYSYMBOL_expr_stmt = 97,                 /* expr_stmt  */
  YYSYMBOL_expr = 98,                      /* expr  */
  YYSYMBOL_assign_expr = 99,               /* assign_expr  */
  YYSYMBOL_ternary_expr = 100,             /* ternary_expr  */
  YYSYMBOL_or_expr = 101,                  /* or_expr  */
  YYSYMBOL_and_expr = 102,                 /* and_expr  */
  YYSYMBOL_bitor_expr = 103,               /* bitor_expr  */
  YYSYMBOL_bitxor_expr = 104,              /* bitxor_expr  */
  YYSYMBOL_bitand_expr = 105,              /* bitand_expr  */
  YYSYMBOL_eq_expr = 106,                  /* eq_expr  */
  YYSYMBOL_rel_expr = 107,                 /* rel_expr  */
  YYSYMBOL_shift_expr = 108,               /* shift_expr  */
  YYSYMBOL_add_expr = 109,                 /* add_expr  */
  YYSYMBOL_mul_expr = 110,                 /* mul_expr  */
  YYSYMBOL_unary_expr = 111,               /* unary_expr  */
  YYSYMBOL_postfix_expr = 112,             /* postfix_expr  */
  YYSYMBOL_primary_expr = 113,             /* primary_expr  */
  YYSYMBOL_arg_list = 114,                 /* arg_list  */
  YYSYMBOL_arg_list_ne = 115               /* arg_list_ne  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   393

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  71
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  131
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  234

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   325


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   112,   112,   118,   119,   124,   125,   126,   134,   135,
     136,   137,   138,   139,   147,   153,   154,   163,   165,   167,
     169,   174,   182,   188,   189,   194,   196,   201,   203,   212,
     218,   219,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   238,   240,   245,   250,   255,   261,   263,
     266,   270,   271,   275,   276,   280,   282,   287,   292,   297,
     306,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   325,   326,   331,   332,   336,   337,   341,
     342,   346,   347,   351,   352,   356,   357,   358,   362,   363,
     364,   365,   366,   370,   371,   372,   376,   377,   378,   382,
     383,   384,   385,   389,   390,   391,   392,   393,   394,   395,
     396,   397,   402,   403,   404,   405,   406,   407,   408,   412,
     413,   414,   415,   416,   417,   418,   419,   420,   425,   426,
     430,   432
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "KW_INT", "KW_FLOAT",
  "KW_CHAR", "KW_BOOL", "KW_VOID", "KW_IF", "KW_ELSE", "KW_WHILE", "KW_DO",
  "KW_FOR", "KW_RETURN", "KW_STRUCT", "KW_BREAK", "KW_CONTINUE",
  "KW_PRINTF", "KW_SCANF", "INT_LITERAL", "BOOL_LITERAL", "FLOAT_LITERAL",
  "CHAR_LITERAL", "STR_LITERAL", "IDENTIFIER", "OP_PLUS", "OP_MINUS",
  "OP_MUL", "OP_DIV", "OP_MOD", "OP_ASSIGN", "OP_PLUS_ASSIGN",
  "OP_MINUS_ASSIGN", "OP_MUL_ASSIGN", "OP_DIV_ASSIGN", "OP_MOD_ASSIGN",
  "OP_AND_ASSIGN", "OP_OR_ASSIGN", "OP_XOR_ASSIGN", "OP_LSHIFT_ASSIGN",
  "OP_RSHIFT_ASSIGN", "OP_EQ", "OP_NEQ", "OP_LT", "OP_GT", "OP_LEQ",
  "OP_GEQ", "OP_AND", "OP_OR", "OP_NOT", "OP_INC", "OP_DEC", "OP_BITAND",
  "OP_BITOR", "OP_BITXOR", "OP_BITNOT", "OP_LSHIFT", "OP_RSHIFT",
  "OP_ARROW", "OP_DOT", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACKET",
  "RBRACKET", "SEMICOLON", "COMMA", "COLON", "QUESTION", "UNARY_PREC",
  "$accept", "program", "translation_unit", "declaration", "type_spec",
  "struct_decl", "declaration_list", "var_decl", "init_opt",
  "function_def", "param_list", "param_list_ne", "param", "compound_stmt",
  "stmt_list", "stmt", "if_stmt", "while_stmt", "do_while_stmt",
  "for_stmt", "for_init", "for_cond", "for_incr", "return_stmt",
  "break_stmt", "continue_stmt", "expr_stmt", "expr", "assign_expr",
  "ternary_expr", "or_expr", "and_expr", "bitor_expr", "bitxor_expr",
  "bitand_expr", "eq_expr", "rel_expr", "shift_expr", "add_expr",
  "mul_expr", "unary_expr", "postfix_expr", "primary_expr", "arg_list",
  "arg_list_ne", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-173)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -173,    12,    81,  -173,  -173,  -173,  -173,  -173,  -173,    -6,
    -173,     6,  -173,  -173,  -173,   -48,   -29,  -173,   333,    93,
      15,  -173,     1,  -173,  -173,  -173,  -173,  -173,  -173,  -173,
    -173,   333,   333,   333,   333,   333,   333,   333,   271,   -30,
    -173,  -173,  -173,   -37,    -3,     8,     3,     7,   -25,    58,
     -28,    25,    27,   151,   -11,  -173,    42,    82,    44,    46,
    -173,    50,    56,   114,  -173,  -173,  -173,  -173,  -173,  -173,
    -173,  -173,    80,    83,  -173,   333,   333,   333,   333,   333,
     333,   333,   333,   333,   333,   333,   333,   333,   333,   333,
     333,   333,   333,   333,   333,   333,   333,   333,   333,   333,
     333,   333,   333,   333,   333,  -173,  -173,   118,   119,   333,
     333,  -173,    85,    84,    93,   -20,  -173,   -21,   333,  -173,
      -3,  -173,    77,     8,     3,     7,   -25,    58,    58,   -28,
     -28,   -28,   -28,    25,    25,    27,    27,  -173,  -173,  -173,
    -173,  -173,  -173,  -173,  -173,  -173,  -173,  -173,  -173,  -173,
    -173,  -173,  -173,  -173,    86,    88,    91,    92,  -173,  -173,
    -173,    89,  -173,  -173,   333,  -173,   333,  -173,  -173,   113,
     333,  -173,  -173,    90,    98,   207,   100,   283,    87,    95,
    -173,  -173,  -173,  -173,  -173,  -173,  -173,  -173,  -173,  -173,
    -173,  -173,  -173,   101,   103,   333,   333,   159,   271,  -173,
     104,  -173,  -173,  -173,   105,   111,   131,   133,   150,   128,
    -173,  -173,  -173,   207,   207,   333,   165,   333,   187,  -173,
     139,   333,   135,  -173,   207,   136,  -173,   333,  -173,  -173,
     143,  -173,   207,  -173
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     8,     9,    10,    11,    12,     0,
       4,     0,     7,     6,     5,    13,     0,    15,     0,    23,
       0,    17,     0,   125,   126,   119,   123,   120,   121,   122,
     124,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      21,    60,    61,    73,    75,    77,    79,    81,    83,    85,
      88,    93,    96,    99,   103,   112,     0,     0,     0,    24,
      25,     0,     0,     0,    16,   104,   110,   105,   107,   108,
     109,   106,     0,     0,    18,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   117,   118,     0,     0,   128,
       0,    13,    27,     0,     0,     0,    14,     0,     0,   127,
      76,    99,     0,    78,    80,    82,    84,    86,    87,    89,
      90,    91,    92,    94,    95,    97,    98,   100,   101,   102,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,   116,   115,   130,     0,   129,     0,     0,    30,    22,
      26,     0,    19,   111,     0,   114,     0,   113,    28,     0,
     128,    74,   131,     0,     0,     0,     0,     0,     0,     0,
      29,    42,    32,    40,    31,    33,    34,    35,    36,    37,
      38,    39,    41,     0,     0,     0,     0,     0,    50,    56,
       0,    57,    58,    59,     0,     0,     0,     0,     0,     0,
      49,    55,    20,     0,     0,     0,     0,    52,    43,    45,
       0,     0,     0,    51,     0,     0,    48,    54,    44,    46,
       0,    53,     0,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -173,  -173,  -173,  -173,     0,  -173,  -173,    11,  -173,  -173,
    -173,  -173,    94,   134,  -173,  -172,  -173,  -173,  -173,  -173,
    -173,  -173,  -173,  -173,  -173,  -173,  -173,   -18,   141,    41,
    -173,   132,   129,   138,   153,   166,   -19,    26,     2,     4,
     -10,  -173,  -173,    78,  -173
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,     2,    10,    63,    12,    22,   182,    39,    14,
      58,    59,    60,   183,   169,   184,   185,   186,   187,   188,
     209,   222,   230,   189,   190,   191,   192,   193,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,   154,   155
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      40,    18,    11,   197,     4,     5,     6,     7,     8,    18,
     161,    75,     3,    13,    17,    56,    81,    82,    15,    57,
      73,    65,    66,    67,    68,    69,    70,    71,    87,    88,
      16,    19,    76,    64,    61,    20,    74,    21,    72,   105,
     106,   218,   219,    20,    77,    21,   162,   107,   108,   109,
      89,    90,   228,   110,    91,    92,    93,    79,   122,    80,
     233,    78,   127,   128,    62,   121,   111,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   137,   138,   139,     4,     5,     6,     7,     8,   133,
     134,   153,   156,   135,   136,     9,     4,     5,     6,     7,
       8,    83,    84,    85,    86,   113,   112,    56,   163,   129,
     130,   131,   132,   114,    57,   115,     4,     5,     6,     7,
       8,   173,   116,   174,   175,   176,   177,    56,   178,   179,
      23,    24,    25,    26,    27,    28,    29,    30,   117,    31,
      32,   118,   151,   152,   119,   164,   158,   165,   172,   157,
     195,   170,   153,   201,   121,   166,   167,   168,   196,   200,
     198,   202,    33,    34,    35,    36,   204,   203,    37,   207,
     211,   212,   213,    38,   216,   158,   180,   205,   206,   181,
     210,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   214,   215,   217,   221,   224,   220,   208,   223,
     225,   227,   229,   226,   232,   171,   123,   120,   160,   231,
       4,     5,     6,     7,     8,   173,   124,   174,   175,   176,
     177,    56,   178,   179,    23,    24,    25,    26,    27,    28,
      29,    30,   125,    31,    32,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   126,   159,   194,     0,
       0,     0,     0,     0,     0,     0,    33,    34,    35,    36,
       0,     0,    37,     0,     0,     0,     0,    38,     0,   158,
       0,     0,     0,   181,     4,     5,     6,     7,     8,     0,
       0,     0,     0,     0,     0,    56,     0,     0,    23,    24,
      25,    26,    27,    28,    29,    30,     0,    31,    32,     0,
      23,    24,    25,    26,    27,    28,    29,    30,     0,    31,
      32,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,    35,    36,     0,     0,    37,     0,     0,     0,
       0,    38,    33,    34,    35,    36,     0,     0,    37,     0,
       0,     0,     0,    38,     0,     0,     0,     0,     0,   199,
      23,    24,    25,    26,    27,    28,    29,    30,     0,    31,
      32,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,    35,    36,     0,     0,    37,     0,
       0,     0,     0,    38
};

static const yytype_int16 yycheck[] =
{
      18,    30,     2,   175,     3,     4,     5,     6,     7,    30,
      30,    48,     0,     2,    62,    14,    41,    42,    24,    19,
      38,    31,    32,    33,    34,    35,    36,    37,    56,    57,
      24,    60,    69,    22,    19,    64,    66,    66,    38,    50,
      51,   213,   214,    64,    47,    66,    66,    58,    59,    60,
      25,    26,   224,    64,    27,    28,    29,    54,    76,    52,
     232,    53,    81,    82,    63,    75,    24,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,     3,     4,     5,     6,     7,    87,
      88,   109,   110,    89,    90,    14,     3,     4,     5,     6,
       7,    43,    44,    45,    46,    61,    24,    14,   118,    83,
      84,    85,    86,    67,   114,    65,     3,     4,     5,     6,
       7,     8,    66,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    24,    26,
      27,    61,    24,    24,    61,    68,    62,    61,   166,    64,
      60,    62,   170,    66,   164,    67,    65,    65,    60,   177,
      60,    66,    49,    50,    51,    52,    63,    66,    55,    10,
      66,    66,    61,    60,    24,    62,    63,   195,   196,    66,
     198,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    61,    60,    66,    30,     9,   215,   198,   217,
      61,    66,    66,   221,    61,   164,    77,    75,   114,   227,
       3,     4,     5,     6,     7,     8,    78,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    79,    26,    27,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,    80,   113,   170,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,
      -1,    -1,    55,    -1,    -1,    -1,    -1,    60,    -1,    62,
      -1,    -1,    -1,    66,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    14,    -1,    -1,    17,    18,
      19,    20,    21,    22,    23,    24,    -1,    26,    27,    -1,
      17,    18,    19,    20,    21,    22,    23,    24,    -1,    26,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    -1,    -1,    55,    -1,    -1,    -1,
      -1,    60,    49,    50,    51,    52,    -1,    -1,    55,    -1,
      -1,    -1,    -1,    60,    -1,    -1,    -1,    -1,    -1,    66,
      17,    18,    19,    20,    21,    22,    23,    24,    -1,    26,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    49,    50,    51,    52,    -1,    -1,    55,    -1,
      -1,    -1,    -1,    60
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    72,    73,     0,     3,     4,     5,     6,     7,    14,
      74,    75,    76,    78,    80,    24,    24,    62,    30,    60,
      64,    66,    77,    17,    18,    19,    20,    21,    22,    23,
      24,    26,    27,    49,    50,    51,    52,    55,    60,    79,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,    14,    75,    81,    82,
      83,    19,    63,    75,    78,   111,   111,   111,   111,   111,
     111,   111,    75,    98,    66,    48,    69,    47,    53,    54,
      52,    41,    42,    43,    44,    45,    46,    56,    57,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    50,    51,    58,    59,    60,
      64,    24,    24,    61,    67,    65,    66,    24,    61,    61,
     102,   111,    98,   103,   104,   105,   106,   107,   107,   108,
     108,   108,   108,   109,   109,   110,   110,   111,   111,   111,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    24,    24,    98,   114,   115,    98,    64,    62,    84,
      83,    30,    66,   111,    68,    61,    67,    65,    65,    85,
      62,   100,    98,     8,    10,    11,    12,    13,    15,    16,
      63,    66,    78,    84,    86,    87,    88,    89,    90,    94,
      95,    96,    97,    98,   114,    60,    60,    86,    60,    66,
      98,    66,    66,    66,    63,    98,    98,    10,    75,    91,
      98,    66,    66,    61,    61,    60,    24,    66,    86,    86,
      98,    30,    92,    98,     9,    61,    98,    66,    86,    66,
      93,    98,    61,    86
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    71,    72,    73,    73,    74,    74,    74,    75,    75,
      75,    75,    75,    75,    76,    77,    77,    78,    78,    78,
      78,    79,    80,    81,    81,    82,    82,    83,    83,    84,
      85,    85,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    87,    87,    88,    89,    90,    91,    91,
      91,    92,    92,    93,    93,    94,    94,    95,    96,    97,
      98,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,   100,   100,   101,   101,   102,   102,   103,
     103,   104,   104,   105,   105,   106,   106,   106,   107,   107,
     107,   107,   107,   108,   108,   108,   109,   109,   109,   110,
     110,   110,   110,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   112,   112,   112,   112,   112,   112,   112,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   114,   114,
     115,   115
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     6,     0,     2,     3,     5,     6,
      10,     1,     6,     0,     1,     1,     3,     2,     4,     3,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     5,     7,     5,     7,     9,     4,     1,
       0,     1,     0,     1,     0,     3,     2,     2,     2,     2,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     5,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     1,     2,     2,     2,     2,     2,     2,
       2,     4,     1,     4,     4,     3,     3,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     0,     1,
       1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: translation_unit  */
#line 113 "parser.y"
        { ast_root = make_program((yyvsp[0].list)); }
#line 1409 "parser.tab.c"
    break;

  case 3: /* translation_unit: %empty  */
#line 118 "parser.y"
        { (yyval.list) = nl_new(); }
#line 1415 "parser.tab.c"
    break;

  case 4: /* translation_unit: translation_unit declaration  */
#line 120 "parser.y"
        { (yyval.list) = nl_append((yyvsp[-1].list), (yyvsp[0].node)); }
#line 1421 "parser.tab.c"
    break;

  case 5: /* declaration: function_def  */
#line 124 "parser.y"
                    { (yyval.node) = (yyvsp[0].node); }
#line 1427 "parser.tab.c"
    break;

  case 6: /* declaration: var_decl  */
#line 125 "parser.y"
                    { (yyval.node) = (yyvsp[0].node); }
#line 1433 "parser.tab.c"
    break;

  case 7: /* declaration: struct_decl  */
#line 126 "parser.y"
                    { (yyval.node) = (yyvsp[0].node); }
#line 1439 "parser.tab.c"
    break;

  case 8: /* type_spec: KW_INT  */
#line 134 "parser.y"
                           { (yyval.node) = make_type("int");   }
#line 1445 "parser.tab.c"
    break;

  case 9: /* type_spec: KW_FLOAT  */
#line 135 "parser.y"
                           { (yyval.node) = make_type("float"); }
#line 1451 "parser.tab.c"
    break;

  case 10: /* type_spec: KW_CHAR  */
#line 136 "parser.y"
                           { (yyval.node) = make_type("char");  }
#line 1457 "parser.tab.c"
    break;

  case 11: /* type_spec: KW_BOOL  */
#line 137 "parser.y"
                           { (yyval.node) = make_type("bool");  }
#line 1463 "parser.tab.c"
    break;

  case 12: /* type_spec: KW_VOID  */
#line 138 "parser.y"
                           { (yyval.node) = make_type("void");  }
#line 1469 "parser.tab.c"
    break;

  case 13: /* type_spec: KW_STRUCT IDENTIFIER  */
#line 139 "parser.y"
                           { (yyval.node) = make_struct_type((yyvsp[0].sval)); }
#line 1475 "parser.tab.c"
    break;

  case 14: /* struct_decl: KW_STRUCT IDENTIFIER LBRACE declaration_list RBRACE SEMICOLON  */
#line 148 "parser.y"
        { (yyval.node) = make_struct_decl((yyvsp[-4].sval), (yyvsp[-2].list)); }
#line 1481 "parser.tab.c"
    break;

  case 15: /* declaration_list: %empty  */
#line 153 "parser.y"
        { (yyval.list) = nl_new(); }
#line 1487 "parser.tab.c"
    break;

  case 16: /* declaration_list: declaration_list var_decl  */
#line 155 "parser.y"
        { (yyval.list) = nl_append((yyvsp[-1].list), (yyvsp[0].node)); }
#line 1493 "parser.tab.c"
    break;

  case 17: /* var_decl: type_spec IDENTIFIER SEMICOLON  */
#line 164 "parser.y"
        { (yyval.node) = make_var_decl((yyvsp[-2].node), (yyvsp[-1].sval), NULL, 0); }
#line 1499 "parser.tab.c"
    break;

  case 18: /* var_decl: type_spec IDENTIFIER OP_ASSIGN init_opt SEMICOLON  */
#line 166 "parser.y"
        { (yyval.node) = make_var_decl((yyvsp[-4].node), (yyvsp[-3].sval), (yyvsp[-1].node),   0); }
#line 1505 "parser.tab.c"
    break;

  case 19: /* var_decl: type_spec IDENTIFIER LBRACKET INT_LITERAL RBRACKET SEMICOLON  */
#line 168 "parser.y"
        { (yyval.node) = make_var_decl((yyvsp[-5].node), (yyvsp[-4].sval), NULL, (yyvsp[-2].ival)); }
#line 1511 "parser.tab.c"
    break;

  case 20: /* var_decl: type_spec IDENTIFIER LBRACKET INT_LITERAL RBRACKET OP_ASSIGN LBRACE arg_list RBRACE SEMICOLON  */
#line 170 "parser.y"
        { (yyval.node) = make_array_decl((yyvsp[-9].node), (yyvsp[-8].sval), (yyvsp[-6].ival), (yyvsp[-2].list)); }
#line 1517 "parser.tab.c"
    break;

  case 21: /* init_opt: expr  */
#line 174 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1523 "parser.tab.c"
    break;

  case 22: /* function_def: type_spec IDENTIFIER LPAREN param_list RPAREN compound_stmt  */
#line 183 "parser.y"
        { (yyval.node) = make_func_def((yyvsp[-5].node), (yyvsp[-4].sval), (yyvsp[-2].list), (yyvsp[0].node)); }
#line 1529 "parser.tab.c"
    break;

  case 23: /* param_list: %empty  */
#line 188 "parser.y"
        { (yyval.list) = nl_new(); }
#line 1535 "parser.tab.c"
    break;

  case 24: /* param_list: param_list_ne  */
#line 190 "parser.y"
        { (yyval.list) = (yyvsp[0].list); }
#line 1541 "parser.tab.c"
    break;

  case 25: /* param_list_ne: param  */
#line 195 "parser.y"
        { (yyval.list) = nl_single((yyvsp[0].node)); }
#line 1547 "parser.tab.c"
    break;

  case 26: /* param_list_ne: param_list_ne COMMA param  */
#line 197 "parser.y"
        { (yyval.list) = nl_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 1553 "parser.tab.c"
    break;

  case 27: /* param: type_spec IDENTIFIER  */
#line 202 "parser.y"
        { (yyval.node) = make_param((yyvsp[-1].node), (yyvsp[0].sval), 0); }
#line 1559 "parser.tab.c"
    break;

  case 28: /* param: type_spec IDENTIFIER LBRACKET RBRACKET  */
#line 204 "parser.y"
        { (yyval.node) = make_param((yyvsp[-3].node), (yyvsp[-2].sval), 1); }
#line 1565 "parser.tab.c"
    break;

  case 29: /* compound_stmt: LBRACE stmt_list RBRACE  */
#line 213 "parser.y"
        { (yyval.node) = make_compound((yyvsp[-1].list)); }
#line 1571 "parser.tab.c"
    break;

  case 30: /* stmt_list: %empty  */
#line 218 "parser.y"
        { (yyval.list) = nl_new(); }
#line 1577 "parser.tab.c"
    break;

  case 31: /* stmt_list: stmt_list stmt  */
#line 220 "parser.y"
        { (yyval.list) = nl_append((yyvsp[-1].list), (yyvsp[0].node)); }
#line 1583 "parser.tab.c"
    break;

  case 32: /* stmt: var_decl  */
#line 224 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1589 "parser.tab.c"
    break;

  case 33: /* stmt: if_stmt  */
#line 225 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1595 "parser.tab.c"
    break;

  case 34: /* stmt: while_stmt  */
#line 226 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1601 "parser.tab.c"
    break;

  case 35: /* stmt: do_while_stmt  */
#line 227 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1607 "parser.tab.c"
    break;

  case 36: /* stmt: for_stmt  */
#line 228 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1613 "parser.tab.c"
    break;

  case 37: /* stmt: return_stmt  */
#line 229 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1619 "parser.tab.c"
    break;

  case 38: /* stmt: break_stmt  */
#line 230 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1625 "parser.tab.c"
    break;

  case 39: /* stmt: continue_stmt  */
#line 231 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1631 "parser.tab.c"
    break;

  case 40: /* stmt: compound_stmt  */
#line 232 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1637 "parser.tab.c"
    break;

  case 41: /* stmt: expr_stmt  */
#line 233 "parser.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1643 "parser.tab.c"
    break;

  case 42: /* stmt: SEMICOLON  */
#line 234 "parser.y"
                        { (yyval.node) = make_empty_stmt(); }
#line 1649 "parser.tab.c"
    break;

  case 43: /* if_stmt: KW_IF LPAREN expr RPAREN stmt  */
#line 239 "parser.y"
        { (yyval.node) = make_if((yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1655 "parser.tab.c"
    break;

  case 44: /* if_stmt: KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt  */
#line 241 "parser.y"
        { (yyval.node) = make_if((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1661 "parser.tab.c"
    break;

  case 45: /* while_stmt: KW_WHILE LPAREN expr RPAREN stmt  */
#line 246 "parser.y"
        { (yyval.node) = make_while((yyvsp[-2].node), (yyvsp[0].node)); }
#line 1667 "parser.tab.c"
    break;

  case 46: /* do_while_stmt: KW_DO stmt KW_WHILE LPAREN expr RPAREN SEMICOLON  */
#line 251 "parser.y"
        { (yyval.node) = make_do_while((yyvsp[-5].node), (yyvsp[-2].node)); }
#line 1673 "parser.tab.c"
    break;

  case 47: /* for_stmt: KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_incr RPAREN stmt  */
#line 256 "parser.y"
        { (yyval.node) = make_for((yyvsp[-6].node), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1679 "parser.tab.c"
    break;

  case 48: /* for_init: type_spec IDENTIFIER OP_ASSIGN expr  */
#line 262 "parser.y"
        { (yyval.node) = make_var_decl((yyvsp[-3].node), (yyvsp[-2].sval), (yyvsp[0].node), 0); }
#line 1685 "parser.tab.c"
    break;

  case 49: /* for_init: expr  */
#line 264 "parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1691 "parser.tab.c"
    break;

  case 50: /* for_init: %empty  */
#line 266 "parser.y"
        { (yyval.node) = NULL; }
#line 1697 "parser.tab.c"
    break;

  case 51: /* for_cond: expr  */
#line 270 "parser.y"
                  { (yyval.node) = (yyvsp[0].node);   }
#line 1703 "parser.tab.c"
    break;

  case 52: /* for_cond: %empty  */
#line 271 "parser.y"
                  { (yyval.node) = NULL; }
#line 1709 "parser.tab.c"
    break;

  case 53: /* for_incr: expr  */
#line 275 "parser.y"
                  { (yyval.node) = (yyvsp[0].node);   }
#line 1715 "parser.tab.c"
    break;

  case 54: /* for_incr: %empty  */
#line 276 "parser.y"
                  { (yyval.node) = NULL; }
#line 1721 "parser.tab.c"
    break;

  case 55: /* return_stmt: KW_RETURN expr SEMICOLON  */
#line 281 "parser.y"
        { (yyval.node) = make_return((yyvsp[-1].node)); }
#line 1727 "parser.tab.c"
    break;

  case 56: /* return_stmt: KW_RETURN SEMICOLON  */
#line 283 "parser.y"
        { (yyval.node) = make_return(NULL); }
#line 1733 "parser.tab.c"
    break;

  case 57: /* break_stmt: KW_BREAK SEMICOLON  */
#line 288 "parser.y"
        { (yyval.node) = make_break(); }
#line 1739 "parser.tab.c"
    break;

  case 58: /* continue_stmt: KW_CONTINUE SEMICOLON  */
#line 293 "parser.y"
        { (yyval.node) = make_continue(); }
#line 1745 "parser.tab.c"
    break;

  case 59: /* expr_stmt: expr SEMICOLON  */
#line 298 "parser.y"
        { (yyval.node) = make_expr_stmt((yyvsp[-1].node)); }
#line 1751 "parser.tab.c"
    break;

  case 60: /* expr: assign_expr  */
#line 306 "parser.y"
                    { (yyval.node) = (yyvsp[0].node); }
#line 1757 "parser.tab.c"
    break;

  case 61: /* assign_expr: ternary_expr  */
#line 310 "parser.y"
                                                { (yyval.node) = (yyvsp[0].node); }
#line 1763 "parser.tab.c"
    break;

  case 62: /* assign_expr: unary_expr OP_ASSIGN assign_expr  */
#line 311 "parser.y"
                                               { (yyval.node) = make_binop("=",   (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1769 "parser.tab.c"
    break;

  case 63: /* assign_expr: unary_expr OP_PLUS_ASSIGN assign_expr  */
#line 312 "parser.y"
                                               { (yyval.node) = make_binop("+=",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1775 "parser.tab.c"
    break;

  case 64: /* assign_expr: unary_expr OP_MINUS_ASSIGN assign_expr  */
#line 313 "parser.y"
                                               { (yyval.node) = make_binop("-=",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1781 "parser.tab.c"
    break;

  case 65: /* assign_expr: unary_expr OP_MUL_ASSIGN assign_expr  */
#line 314 "parser.y"
                                               { (yyval.node) = make_binop("*=",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1787 "parser.tab.c"
    break;

  case 66: /* assign_expr: unary_expr OP_DIV_ASSIGN assign_expr  */
#line 315 "parser.y"
                                               { (yyval.node) = make_binop("/=",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1793 "parser.tab.c"
    break;

  case 67: /* assign_expr: unary_expr OP_MOD_ASSIGN assign_expr  */
#line 316 "parser.y"
                                               { (yyval.node) = make_binop("%=",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1799 "parser.tab.c"
    break;

  case 68: /* assign_expr: unary_expr OP_AND_ASSIGN assign_expr  */
#line 317 "parser.y"
                                               { (yyval.node) = make_binop("&=",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1805 "parser.tab.c"
    break;

  case 69: /* assign_expr: unary_expr OP_OR_ASSIGN assign_expr  */
#line 318 "parser.y"
                                               { (yyval.node) = make_binop("|=",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1811 "parser.tab.c"
    break;

  case 70: /* assign_expr: unary_expr OP_XOR_ASSIGN assign_expr  */
#line 319 "parser.y"
                                               { (yyval.node) = make_binop("^=",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1817 "parser.tab.c"
    break;

  case 71: /* assign_expr: unary_expr OP_LSHIFT_ASSIGN assign_expr  */
#line 320 "parser.y"
                                               { (yyval.node) = make_binop("<<=", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1823 "parser.tab.c"
    break;

  case 72: /* assign_expr: unary_expr OP_RSHIFT_ASSIGN assign_expr  */
#line 321 "parser.y"
                                               { (yyval.node) = make_binop(">>=", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1829 "parser.tab.c"
    break;

  case 73: /* ternary_expr: or_expr  */
#line 325 "parser.y"
                                            { (yyval.node) = (yyvsp[0].node); }
#line 1835 "parser.tab.c"
    break;

  case 74: /* ternary_expr: or_expr QUESTION expr COLON ternary_expr  */
#line 327 "parser.y"
        { (yyval.node) = make_ternary((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1841 "parser.tab.c"
    break;

  case 75: /* or_expr: and_expr  */
#line 331 "parser.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 1847 "parser.tab.c"
    break;

  case 76: /* or_expr: or_expr OP_OR and_expr  */
#line 332 "parser.y"
                                    { (yyval.node) = make_binop("||", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1853 "parser.tab.c"
    break;

  case 77: /* and_expr: bitor_expr  */
#line 336 "parser.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 1859 "parser.tab.c"
    break;

  case 78: /* and_expr: and_expr OP_AND bitor_expr  */
#line 337 "parser.y"
                                    { (yyval.node) = make_binop("&&", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1865 "parser.tab.c"
    break;

  case 79: /* bitor_expr: bitxor_expr  */
#line 341 "parser.y"
                                        { (yyval.node) = (yyvsp[0].node); }
#line 1871 "parser.tab.c"
    break;

  case 80: /* bitor_expr: bitor_expr OP_BITOR bitxor_expr  */
#line 342 "parser.y"
                                        { (yyval.node) = make_binop("|", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1877 "parser.tab.c"
    break;

  case 81: /* bitxor_expr: bitand_expr  */
#line 346 "parser.y"
                                        { (yyval.node) = (yyvsp[0].node); }
#line 1883 "parser.tab.c"
    break;

  case 82: /* bitxor_expr: bitxor_expr OP_BITXOR bitand_expr  */
#line 347 "parser.y"
                                        { (yyval.node) = make_binop("^", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1889 "parser.tab.c"
    break;

  case 83: /* bitand_expr: eq_expr  */
#line 351 "parser.y"
                                        { (yyval.node) = (yyvsp[0].node); }
#line 1895 "parser.tab.c"
    break;

  case 84: /* bitand_expr: bitand_expr OP_BITAND eq_expr  */
#line 352 "parser.y"
                                        { (yyval.node) = make_binop("&", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1901 "parser.tab.c"
    break;

  case 85: /* eq_expr: rel_expr  */
#line 356 "parser.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 1907 "parser.tab.c"
    break;

  case 86: /* eq_expr: eq_expr OP_EQ rel_expr  */
#line 357 "parser.y"
                                    { (yyval.node) = make_binop("==", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1913 "parser.tab.c"
    break;

  case 87: /* eq_expr: eq_expr OP_NEQ rel_expr  */
#line 358 "parser.y"
                                    { (yyval.node) = make_binop("!=", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1919 "parser.tab.c"
    break;

  case 88: /* rel_expr: shift_expr  */
#line 362 "parser.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 1925 "parser.tab.c"
    break;

  case 89: /* rel_expr: rel_expr OP_LT shift_expr  */
#line 363 "parser.y"
                                    { (yyval.node) = make_binop("<",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1931 "parser.tab.c"
    break;

  case 90: /* rel_expr: rel_expr OP_GT shift_expr  */
#line 364 "parser.y"
                                    { (yyval.node) = make_binop(">",  (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1937 "parser.tab.c"
    break;

  case 91: /* rel_expr: rel_expr OP_LEQ shift_expr  */
#line 365 "parser.y"
                                    { (yyval.node) = make_binop("<=", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1943 "parser.tab.c"
    break;

  case 92: /* rel_expr: rel_expr OP_GEQ shift_expr  */
#line 366 "parser.y"
                                    { (yyval.node) = make_binop(">=", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1949 "parser.tab.c"
    break;

  case 93: /* shift_expr: add_expr  */
#line 370 "parser.y"
                                        { (yyval.node) = (yyvsp[0].node); }
#line 1955 "parser.tab.c"
    break;

  case 94: /* shift_expr: shift_expr OP_LSHIFT add_expr  */
#line 371 "parser.y"
                                        { (yyval.node) = make_binop("<<", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1961 "parser.tab.c"
    break;

  case 95: /* shift_expr: shift_expr OP_RSHIFT add_expr  */
#line 372 "parser.y"
                                        { (yyval.node) = make_binop(">>", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1967 "parser.tab.c"
    break;

  case 96: /* add_expr: mul_expr  */
#line 376 "parser.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 1973 "parser.tab.c"
    break;

  case 97: /* add_expr: add_expr OP_PLUS mul_expr  */
#line 377 "parser.y"
                                    { (yyval.node) = make_binop("+", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1979 "parser.tab.c"
    break;

  case 98: /* add_expr: add_expr OP_MINUS mul_expr  */
#line 378 "parser.y"
                                    { (yyval.node) = make_binop("-", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1985 "parser.tab.c"
    break;

  case 99: /* mul_expr: unary_expr  */
#line 382 "parser.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 1991 "parser.tab.c"
    break;

  case 100: /* mul_expr: mul_expr OP_MUL unary_expr  */
#line 383 "parser.y"
                                    { (yyval.node) = make_binop("*", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1997 "parser.tab.c"
    break;

  case 101: /* mul_expr: mul_expr OP_DIV unary_expr  */
#line 384 "parser.y"
                                    { (yyval.node) = make_binop("/", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2003 "parser.tab.c"
    break;

  case 102: /* mul_expr: mul_expr OP_MOD unary_expr  */
#line 385 "parser.y"
                                    { (yyval.node) = make_binop("%", (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2009 "parser.tab.c"
    break;

  case 103: /* unary_expr: postfix_expr  */
#line 389 "parser.y"
                                            { (yyval.node) = (yyvsp[0].node); }
#line 2015 "parser.tab.c"
    break;

  case 104: /* unary_expr: OP_MINUS unary_expr  */
#line 390 "parser.y"
                                            { (yyval.node) = make_unop("neg", (yyvsp[0].node));   }
#line 2021 "parser.tab.c"
    break;

  case 105: /* unary_expr: OP_NOT unary_expr  */
#line 391 "parser.y"
                                            { (yyval.node) = make_unop("!",   (yyvsp[0].node));   }
#line 2027 "parser.tab.c"
    break;

  case 106: /* unary_expr: OP_BITNOT unary_expr  */
#line 392 "parser.y"
                                            { (yyval.node) = make_unop("~",   (yyvsp[0].node));   }
#line 2033 "parser.tab.c"
    break;

  case 107: /* unary_expr: OP_INC unary_expr  */
#line 393 "parser.y"
                                            { (yyval.node) = make_unop("pre++", (yyvsp[0].node)); }
#line 2039 "parser.tab.c"
    break;

  case 108: /* unary_expr: OP_DEC unary_expr  */
#line 394 "parser.y"
                                            { (yyval.node) = make_unop("pre--", (yyvsp[0].node)); }
#line 2045 "parser.tab.c"
    break;

  case 109: /* unary_expr: OP_BITAND unary_expr  */
#line 395 "parser.y"
                                            { (yyval.node) = make_unop("addr", (yyvsp[0].node));  }
#line 2051 "parser.tab.c"
    break;

  case 110: /* unary_expr: OP_MUL unary_expr  */
#line 396 "parser.y"
                                            { (yyval.node) = make_unop("deref", (yyvsp[0].node)); }
#line 2057 "parser.tab.c"
    break;

  case 111: /* unary_expr: LPAREN type_spec RPAREN unary_expr  */
#line 398 "parser.y"
        { (yyval.node) = make_cast((yyvsp[-2].node), (yyvsp[0].node)); }
#line 2063 "parser.tab.c"
    break;

  case 112: /* postfix_expr: primary_expr  */
#line 402 "parser.y"
                                                    { (yyval.node) = (yyvsp[0].node); }
#line 2069 "parser.tab.c"
    break;

  case 113: /* postfix_expr: postfix_expr LBRACKET expr RBRACKET  */
#line 403 "parser.y"
                                                    { (yyval.node) = make_index((yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2075 "parser.tab.c"
    break;

  case 114: /* postfix_expr: postfix_expr LPAREN arg_list RPAREN  */
#line 404 "parser.y"
                                                    { (yyval.node) = make_call((yyvsp[-3].node), (yyvsp[-1].list)); }
#line 2081 "parser.tab.c"
    break;

  case 115: /* postfix_expr: postfix_expr OP_DOT IDENTIFIER  */
#line 405 "parser.y"
                                                    { (yyval.node) = make_member((yyvsp[-2].node), (yyvsp[0].sval), 0); }
#line 2087 "parser.tab.c"
    break;

  case 116: /* postfix_expr: postfix_expr OP_ARROW IDENTIFIER  */
#line 406 "parser.y"
                                                    { (yyval.node) = make_member((yyvsp[-2].node), (yyvsp[0].sval), 1); }
#line 2093 "parser.tab.c"
    break;

  case 117: /* postfix_expr: postfix_expr OP_INC  */
#line 407 "parser.y"
                                                    { (yyval.node) = make_unop("post++", (yyvsp[-1].node)); }
#line 2099 "parser.tab.c"
    break;

  case 118: /* postfix_expr: postfix_expr OP_DEC  */
#line 408 "parser.y"
                                                    { (yyval.node) = make_unop("post--", (yyvsp[-1].node)); }
#line 2105 "parser.tab.c"
    break;

  case 119: /* primary_expr: INT_LITERAL  */
#line 412 "parser.y"
                        { (yyval.node) = make_int_lit((yyvsp[0].ival));   }
#line 2111 "parser.tab.c"
    break;

  case 120: /* primary_expr: FLOAT_LITERAL  */
#line 413 "parser.y"
                        { (yyval.node) = make_float_lit((yyvsp[0].fval)); }
#line 2117 "parser.tab.c"
    break;

  case 121: /* primary_expr: CHAR_LITERAL  */
#line 414 "parser.y"
                        { (yyval.node) = make_char_lit((yyvsp[0].cval));  }
#line 2123 "parser.tab.c"
    break;

  case 122: /* primary_expr: STR_LITERAL  */
#line 415 "parser.y"
                        { (yyval.node) = make_str_lit((yyvsp[0].sval));   }
#line 2129 "parser.tab.c"
    break;

  case 123: /* primary_expr: BOOL_LITERAL  */
#line 416 "parser.y"
                        { (yyval.node) = make_bool_lit((yyvsp[0].ival));  }
#line 2135 "parser.tab.c"
    break;

  case 124: /* primary_expr: IDENTIFIER  */
#line 417 "parser.y"
                        { (yyval.node) = make_ident((yyvsp[0].sval));     }
#line 2141 "parser.tab.c"
    break;

  case 125: /* primary_expr: KW_PRINTF  */
#line 418 "parser.y"
                        { (yyval.node) = make_ident("printf"); }
#line 2147 "parser.tab.c"
    break;

  case 126: /* primary_expr: KW_SCANF  */
#line 419 "parser.y"
                        { (yyval.node) = make_ident("scanf");  }
#line 2153 "parser.tab.c"
    break;

  case 127: /* primary_expr: LPAREN expr RPAREN  */
#line 420 "parser.y"
                         { (yyval.node) = (yyvsp[-1].node); }
#line 2159 "parser.tab.c"
    break;

  case 128: /* arg_list: %empty  */
#line 425 "parser.y"
                    { (yyval.list) = nl_new(); }
#line 2165 "parser.tab.c"
    break;

  case 129: /* arg_list: arg_list_ne  */
#line 426 "parser.y"
                    { (yyval.list) = (yyvsp[0].list); }
#line 2171 "parser.tab.c"
    break;

  case 130: /* arg_list_ne: expr  */
#line 431 "parser.y"
        { (yyval.list) = nl_single((yyvsp[0].node)); }
#line 2177 "parser.tab.c"
    break;

  case 131: /* arg_list_ne: arg_list_ne COMMA expr  */
#line 433 "parser.y"
        { (yyval.list) = nl_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 2183 "parser.tab.c"
    break;


#line 2187 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 436 "parser.y"

