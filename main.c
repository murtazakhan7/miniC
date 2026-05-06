/*
 * main.c — MiniC Compiler Driver
 *
 * Usage:
 *   ./minic [options] <source.c>
 *   ./minic --tokens <source.c>                 — print token stream only
 *   ./minic --ast <source.c>                    — print AST + run semantic analysis
 *   ./minic --emit-ir --emit-tac <source.c>     — print IR text + final TAC
 *   ./minic --no-opt --emit-tac <source.c>      — print unoptimized TAC
 *   ./minic --codegen -o out.s <source.c>       — emit x86 assembly to file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"
#include "ir.h"
#include "backend.h"

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
        fprintf(stderr, "Usage: %s [options] <source.c>\n", argv[0]);
        return 1;
    }

    int tokens_only = 0;
    int print_ast = 0;
    int build_ir = 1;
    int build_tac = 1;
    int optimize_tac = 1;
    int emit_ir = 0;
    int emit_tac = 1;
    int run_codegen = 0;
    const char *filename = NULL;
    const char *output_path = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tokens") == 0) tokens_only = 1;
        else if (strcmp(argv[i], "--ast") == 0) { tokens_only = 0; print_ast = 1; }
        else if (strcmp(argv[i], "--emit-ir") == 0) emit_ir = 1;
        else if (strcmp(argv[i], "--emit-tac") == 0) emit_tac = 1;
        else if (strcmp(argv[i], "--no-emit-tac") == 0) emit_tac = 0;
        else if (strcmp(argv[i], "--no-ir") == 0) build_ir = 0;
        else if (strcmp(argv[i], "--no-tac") == 0) build_tac = 0;
        else if (strcmp(argv[i], "--no-opt") == 0) optimize_tac = 0;
        else if (strcmp(argv[i], "--codegen") == 0) run_codegen = 1;
        else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) && i + 1 < argc) {
            output_path = argv[++i];
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: unknown option '%s'\n", argv[i]);
            return 1;
        } else {
            filename = argv[i];
        }
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

    if (!build_ir) {
        build_tac = 0;
        optimize_tac = 0;
        run_codegen = 0;
        emit_tac = 0;
    }
    if (!build_tac) {
        optimize_tac = 0;
        run_codegen = 0;
        emit_tac = 0;
    }
    if (emit_ir && !build_ir) {
        fprintf(stderr, "Error: --emit-ir requires IR building\n");
        return 1;
    }

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

        if (print_ast) {
            printf("=== Abstract Syntax Tree ===\n");
            ast_print(ast_root, 0);
        }

        printf("\n=== Semantic Analysis ===\n");
        if (semantic_analyze(ast_root) != 0) {
            ast_free(ast_root);
            fprintf(stderr, "Compilation failed (semantic errors).\n");
            return 1;
        }

        printf("Semantic analysis successful.\n\n");

        FILE *pipeline_out = stdout;
        FILE *asm_out = stdout;
        char *report_path = NULL;

        if (output_path && run_codegen) {
            asm_out = fopen(output_path, "w"); /* pure assembly output */
            if (!asm_out) {
                perror(output_path);
                ast_free(ast_root);
                return 1;
            }

            if (emit_ir || emit_tac) {
                size_t report_len = strlen(output_path) + strlen(".log") + 1;
                report_path = (char *)malloc(report_len);
                if (!report_path) {
                    fprintf(stderr, "Error: failed to allocate report path buffer\n");
                    fclose(asm_out);
                    ast_free(ast_root);
                    return 1;
                }
                snprintf(report_path, report_len, "%s.log", output_path);
                pipeline_out = fopen(report_path, "w");
                if (!pipeline_out) {
                    perror(report_path);
                    free(report_path);
                    fclose(asm_out);
                    ast_free(ast_root);
                    return 1;
                }
            }
        } else if (output_path) {
            pipeline_out = fopen(output_path, "w"); /* overwrite if exists */
            if (!pipeline_out) {
                perror(output_path);
                ast_free(ast_root);
                return 1;
            }
        }

        if (build_ir) {
            FILE *ir_out = emit_ir ? pipeline_out : tmpfile();
            ir_generate(ast_root, ir_out ? ir_out : pipeline_out);
            if (!emit_ir && ir_out) fclose(ir_out);
        }

        if (build_tac && ir_tac_program) {
            if (optimize_tac) {
                tac_optimize(ir_tac_program);
            }
            if (emit_tac) {
                fprintf(pipeline_out, "=== Three Address Code (TAC) — %s ===\n\n",
                        optimize_tac ? "Optimized" : "Unoptimized");
                tac_print(ir_tac_program, pipeline_out);
            }
        }

        if (run_codegen && ir_tac_program) {
            tac_codegen_x86(ir_tac_program, asm_out);
        }

        if (pipeline_out != stdout) {
            fclose(pipeline_out);
        }
        if (asm_out != stdout) {
            fclose(asm_out);
        }
        if (report_path) {
            printf("Wrote IR/TAC report to %s\n", report_path);
            free(report_path);
        }
        
        ast_free(ast_root);
        printf("\nCompilation successful.\n");
    }

    return 0;
}
