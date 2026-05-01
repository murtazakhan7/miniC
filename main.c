/*
 * main.c — MiniC Compiler Driver
 *
 * Usage:
 *   ./minic <source.c>            — compile and print AST
 *   ./minic --tokens <source.c>   — print token stream only (lexer mode)
 *   ./minic --ast    <source.c>   — print AST (default)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Bison/Flex externals */
extern FILE    *yyin;
extern int      yyparse(void);
extern int      yylex(void);
extern char    *yytext;
extern int      line_num;
extern ASTNode *ast_root;

/* Token names for --tokens mode (must match parser.tab.h order) */
#include "parser.tab.h"

static const char *tok_name(int tok) {
    switch (tok) {
    case KW_INT:      return "KW_INT";
    case KW_FLOAT:    return "KW_FLOAT";
    case KW_CHAR:     return "KW_CHAR";
    case KW_BOOL:     return "KW_BOOL";
    case KW_VOID:     return "KW_VOID";
    case KW_IF:       return "KW_IF";
    case KW_ELSE:     return "KW_ELSE";
    case KW_WHILE:    return "KW_WHILE";
    case KW_DO:       return "KW_DO";
    case KW_FOR:      return "KW_FOR";
    case KW_RETURN:   return "KW_RETURN";
    case KW_STRUCT:   return "KW_STRUCT";
    case KW_BREAK:    return "KW_BREAK";
    case KW_CONTINUE: return "KW_CONTINUE";
    case KW_PRINTF:   return "KW_PRINTF";
    case KW_SCANF:    return "KW_SCANF";
    case IDENTIFIER:  return "IDENTIFIER";
    case INT_LITERAL: return "INT_LITERAL";
    case FLOAT_LITERAL: return "FLOAT_LITERAL";
    case CHAR_LITERAL:  return "CHAR_LITERAL";
    case STR_LITERAL:   return "STR_LITERAL";
    case BOOL_LITERAL:  return "BOOL_LITERAL";
    case OP_PLUS:     return "OP_PLUS";
    case OP_MINUS:    return "OP_MINUS";
    case OP_MUL:      return "OP_MUL";
    case OP_DIV:      return "OP_DIV";
    case OP_MOD:      return "OP_MOD";
    case OP_ASSIGN:   return "OP_ASSIGN";
    case OP_EQ:       return "OP_EQ";
    case OP_NEQ:      return "OP_NEQ";
    case OP_LT:       return "OP_LT";
    case OP_GT:       return "OP_GT";
    case OP_LEQ:      return "OP_LEQ";
    case OP_GEQ:      return "OP_GEQ";
    case OP_AND:      return "OP_AND";
    case OP_OR:       return "OP_OR";
    case OP_NOT:      return "OP_NOT";
    case OP_INC:      return "OP_INC";
    case OP_DEC:      return "OP_DEC";
    case LPAREN:      return "LPAREN";
    case RPAREN:      return "RPAREN";
    case LBRACE:      return "LBRACE";
    case RBRACE:      return "RBRACE";
    case LBRACKET:    return "LBRACKET";
    case RBRACKET:    return "RBRACKET";
    case SEMICOLON:   return "SEMICOLON";
    case COMMA:       return "COMMA";
    default:          return "OTHER";
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [--tokens|--ast] <source.c>\n", argv[0]);
        return 1;
    }

    int tokens_only = 0;
    const char *filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tokens") == 0) tokens_only = 1;
        else if (strcmp(argv[i], "--ast") == 0) tokens_only = 0;
        else filename = argv[i];
    }

    if (!filename) {
        fprintf(stderr, "Error: no source file specified\n");
        return 1;
    }

    yyin = fopen(filename, "r");
    if (!yyin) {
        perror(filename);
        return 1;
    }

    printf("=== MiniC Compiler ===\n");
    printf("Source: %s\n\n", filename);

    if (tokens_only) {
        /* ── Lexer-only mode ── */
        printf("%-5s %-20s %s\n", "LINE", "TOKEN", "LEXEME");
        printf("%-5s %-20s %s\n", "----", "-----", "------");
        int tok;
        while ((tok = yylex()) != 0) {
            printf("%-5d %-20s %s\n", line_num, tok_name(tok), yytext);
        }
    } else {
        /* ── Full parse + AST ── */
        int result = yyparse();
        fclose(yyin);

        if (result != 0) {
            fprintf(stderr, "Compilation failed (parse errors).\n");
            return 1;
        }

        printf("=== Abstract Syntax Tree ===\n");
        ast_print(ast_root, 0);
        ast_free(ast_root);
        printf("\nParsing successful.\n");
    }

    return 0;
}
