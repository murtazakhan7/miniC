/*
 * codegen.c — x86 assembly code generation from TAC
 *
 * Lowers optimized TAC to x86-64 assembly suitable for Linux.
 * Includes calling convention, stack frame setup, and instruction selection.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backend.h"
#include "regalloc.h"

/* =========================================================================
   x86 Code Generator Context
   ========================================================================= */

typedef struct {
    FILE *out;
    TACProgram *prog;
    int label_counter;
    RegAllocResult *alloc;
} CodeGenCtx;

/* =========================================================================
   Operand to Register Conversion
   ========================================================================= */

static void operand_to_code(CodeGenCtx *ctx, const Operand *op, char *buf, size_t bufsize) {
    if (!op || !buf) return;
    
    switch (op->kind) {
    case OPERAND_CONST_INT:
        snprintf(buf, bufsize, "$%d", op->int_val);
        break;
    case OPERAND_CONST_FLOAT:
        snprintf(buf, bufsize, "$%g", op->float_val);
        break;
    case OPERAND_CONST_BOOL:
        snprintf(buf, bufsize, "$%d", op->int_val);
        break;
    case OPERAND_CONST_CHAR:
        snprintf(buf, bufsize, "$%d", (int)(unsigned char)op->int_val);
        break;
    case OPERAND_TEMP:
        if (ctx->alloc) {
            const char *reg = regalloc_get_register(ctx->alloc, op->name);
            snprintf(buf, bufsize, "%%%s", reg);
        } else {
            snprintf(buf, bufsize, "%%eax");
        }
        break;
    case OPERAND_VAR:
        snprintf(buf, bufsize, "-%s(%%rbp)", op->name); /* simplified: treat as stack */
        break;
    case OPERAND_LABEL:
        snprintf(buf, bufsize, "%s", op->name);
        break;
    case OPERAND_STRING:
        snprintf(buf, bufsize, ".LC%d", (int)strlen(op->str_val)); /* simplified: ref to string literal */
        break;
    default:
        snprintf(buf, bufsize, "%%eax");
    }
}

/* =========================================================================
   Function Prologue/Epilogue
   ========================================================================= */

static void gen_function_prologue(CodeGenCtx *ctx, const char *func_name) {
    fprintf(ctx->out, ".globl %s\n", func_name);
    fprintf(ctx->out, ".type %s, @function\n", func_name);
    fprintf(ctx->out, "%s:\n", func_name);
    fprintf(ctx->out, "    push %%rbp\n");
    fprintf(ctx->out, "    mov %%rsp, %%rbp\n");
    fprintf(ctx->out, "    sub $128, %%rsp\n");  /* allocate 128 bytes for locals */
}

static void gen_function_epilogue(CodeGenCtx *ctx) {
    fprintf(ctx->out, "    leave\n");
    fprintf(ctx->out, "    ret\n\n");
}

/* =========================================================================
   Instruction Lowering
   ========================================================================= */

static void gen_instr(CodeGenCtx *ctx, TACInstr *instr) {
    if (!instr) return;
    
    char src[256], dst[256], op1[256], op2[256];
    
    switch (instr->op) {
    case TAC_ASSIGN:
        operand_to_code(ctx, &instr->left, src, sizeof(src));
        operand_to_code(ctx, &instr->dest, dst, sizeof(dst));
        fprintf(ctx->out, "    mov %s, %s\n", src, dst);
        break;
        
    case TAC_ASSIGN_CONST:
        operand_to_code(ctx, &instr->left, src, sizeof(src));
        operand_to_code(ctx, &instr->dest, dst, sizeof(dst));
        fprintf(ctx->out, "    mov %s, %s\n", src, dst);
        break;
        
    case TAC_BINOP: {
        operand_to_code(ctx, &instr->left, op1, sizeof(op1));
        operand_to_code(ctx, &instr->right, op2, sizeof(op2));
        operand_to_code(ctx, &instr->dest, dst, sizeof(dst));
        
        /* Simplified: assume we can use eax/edx as scratch */
        if (strcmp(instr->attr, "+") == 0) {
            fprintf(ctx->out, "    mov %s, %%eax\n", op1);
            fprintf(ctx->out, "    add %s, %%eax\n", op2);
            fprintf(ctx->out, "    mov %%eax, %s\n", dst);
        } else if (strcmp(instr->attr, "-") == 0) {
            fprintf(ctx->out, "    mov %s, %%eax\n", op1);
            fprintf(ctx->out, "    sub %s, %%eax\n", op2);
            fprintf(ctx->out, "    mov %%eax, %s\n", dst);
        } else if (strcmp(instr->attr, "*") == 0) {
            fprintf(ctx->out, "    mov %s, %%eax\n", op1);
            fprintf(ctx->out, "    imul %s, %%eax\n", op2);
            fprintf(ctx->out, "    mov %%eax, %s\n", dst);
        } else if (strcmp(instr->attr, "/") == 0) {
            fprintf(ctx->out, "    mov %s, %%eax\n", op1);
            fprintf(ctx->out, "    cqo\n");
            fprintf(ctx->out, "    idiv %s\n", op2);
            fprintf(ctx->out, "    mov %%eax, %s\n", dst);
        } else if (strcmp(instr->attr, "<") == 0 || strcmp(instr->attr, ">") == 0 ||
                   strcmp(instr->attr, "<=") == 0 || strcmp(instr->attr, ">=") == 0 ||
                   strcmp(instr->attr, "==") == 0 || strcmp(instr->attr, "!=") == 0) {
            fprintf(ctx->out, "    cmp %s, %s\n", op2, op1);
            fprintf(ctx->out, "    set ");
            if (strcmp(instr->attr, "<") == 0) fprintf(ctx->out, "l");
            else if (strcmp(instr->attr, ">") == 0) fprintf(ctx->out, "g");
            else if (strcmp(instr->attr, "<=") == 0) fprintf(ctx->out, "le");
            else if (strcmp(instr->attr, ">=") == 0) fprintf(ctx->out, "ge");
            else if (strcmp(instr->attr, "==") == 0) fprintf(ctx->out, "e");
            else fprintf(ctx->out, "ne");
            fprintf(ctx->out, " %%al\n");
            fprintf(ctx->out, "    movzx %%al, %%eax\n");
            fprintf(ctx->out, "    mov %%eax, %s\n", dst);
        } else {
            fprintf(ctx->out, "    mov %s, %%eax\n", op1);
        }
        break;
    }
        
    case TAC_LABEL:
        operand_to_code(ctx, &instr->dest, src, sizeof(src));
        fprintf(ctx->out, "%s:\n", src);
        break;
        
    case TAC_JUMP:
        operand_to_code(ctx, &instr->dest, src, sizeof(src));
        fprintf(ctx->out, "    jmp %s\n", src);
        break;
        
    case TAC_COND_JUMP:
        operand_to_code(ctx, &instr->left, src, sizeof(src));
        operand_to_code(ctx, &instr->dest, dst, sizeof(dst));
        fprintf(ctx->out, "    cmp $0, %s\n", src);
        fprintf(ctx->out, "    jne %s\n", dst);
        break;
        
    case TAC_CALL: {
        operand_to_code(ctx, &instr->left, src, sizeof(src));
        fprintf(ctx->out, "    call %s\n", src);
        if (instr->dest.kind != OPERAND_INVALID) {
            operand_to_code(ctx, &instr->dest, dst, sizeof(dst));
            fprintf(ctx->out, "    mov %%eax, %s\n", dst);
        }
        break;
    }
        
    case TAC_PARAM:
        operand_to_code(ctx, &instr->left, src, sizeof(src));
        fprintf(ctx->out, "    mov %s, %%edi\n", src);  /* x86-64 calling convention: first arg in %%rdi */
        break;
        
    case TAC_RETURN:
        if (instr->left.kind != OPERAND_INVALID) {
            operand_to_code(ctx, &instr->left, src, sizeof(src));
            fprintf(ctx->out, "    mov %s, %%eax\n", src);
        }
        fprintf(ctx->out, "    jmp .L_return\n");
        break;
        
    case TAC_VAR:
        /* Variable declaration: no code needed (handled in prologue) */
        break;
        
    default:
        /* Other ops: ignored for now */
        break;
    }
}

/* =========================================================================
   Function Lowering
   ========================================================================= */

static void gen_function(CodeGenCtx *ctx, TACFunc *func) {
    if (!func) return;
    
    gen_function_prologue(ctx, func->name);
    
    /* Generate code for each instruction */
    for (int i = 0; i < func->instr_count; i++) {
        gen_instr(ctx, func->instrs[i]);
    }
    
    /* Return label and epilogue */
    fprintf(ctx->out, ".L_return:\n");
    gen_function_epilogue(ctx);
}

/* =========================================================================
   Program-level Codegen
   ========================================================================= */

void tac_codegen_x86(TACProgram *prog, FILE *out) {
    if (!prog || !out) return;
    
    CodeGenCtx ctx = {0};
    ctx.out = out;
    ctx.prog = prog;
    
    fprintf(out, ".section .text\n");
    fprintf(out, ".align 4\n\n");
    
    /* Generate code for each function */
    for (int i = 0; i < prog->func_count; i++) {
        gen_function(&ctx, prog->funcs[i]);
    }
    
    fprintf(out, ".section .data\n");
    fprintf(out, ".align 4\n\n");
    
    /* TODO: emit string literals and global data */
    
    fprintf(out, ".section .rodata\n");
    fprintf(out, ".align 4\n\n");
}
