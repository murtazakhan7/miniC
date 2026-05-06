/*
 * codegen.c — x86 assembly code generation from TAC
 *
 * Lowers optimized TAC to x86-64 assembly suitable for Linux.
 * Includes calling convention, stack frame setup, and instruction selection.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "backend.h"

typedef struct {
    char *name;
    int offset;
} StackSlot;

typedef struct {
    char *value;
    char label[32];
} StringLiteral;

typedef struct {
    FILE *out;
    TACProgram *prog;
    TACFunc *current_func;
    char return_label[128];

    StackSlot *slots;
    int slot_count;
    int slot_cap;

    StringLiteral *strings;
    int string_count;
    int string_cap;

    Operand pending_params[16];
    int pending_param_count;
} CodeGenCtx;

static int align16(int n) { return (n + 15) & ~15; }

static void ensure_slot_cap(CodeGenCtx *ctx) {
    if (ctx->slot_count < ctx->slot_cap) return;
    ctx->slot_cap = (ctx->slot_cap == 0) ? 32 : (ctx->slot_cap * 2);
    ctx->slots = realloc(ctx->slots, (size_t)ctx->slot_cap * sizeof(StackSlot));
    if (!ctx->slots) {
        perror("realloc(stack slots)");
        exit(1);
    }
}

static void ensure_string_cap(CodeGenCtx *ctx) {
    if (ctx->string_count < ctx->string_cap) return;
    ctx->string_cap = (ctx->string_cap == 0) ? 16 : (ctx->string_cap * 2);
    ctx->strings = realloc(ctx->strings, (size_t)ctx->string_cap * sizeof(StringLiteral));
    if (!ctx->strings) {
        perror("realloc(string literals)");
        exit(1);
    }
}

static int slot_index_for(CodeGenCtx *ctx, const char *name) {
    if (!name) return -1;
    for (int i = 0; i < ctx->slot_count; i++) {
        if (strcmp(ctx->slots[i].name, name) == 0) return i;
    }
    return -1;
}

static void add_slot_for_name(CodeGenCtx *ctx, const char *name, int *next_offset) {
    if (!name || slot_index_for(ctx, name) >= 0) return;
    ensure_slot_cap(ctx);
    ctx->slots[ctx->slot_count].name = (char *)name;
    ctx->slots[ctx->slot_count].offset = *next_offset;
    ctx->slot_count++;
    *next_offset += 4;
}

static void add_slot_for_operand(CodeGenCtx *ctx, const Operand *op, int *next_offset) {
    if (!op) return;
    if (op->kind == OPERAND_VAR || op->kind == OPERAND_TEMP) {
        add_slot_for_name(ctx, op->name, next_offset);
    }
}

static int get_stack_offset(CodeGenCtx *ctx, const Operand *op) {
    if (!op || (op->kind != OPERAND_VAR && op->kind != OPERAND_TEMP)) return -1;
    int idx = slot_index_for(ctx, op->name);
    return (idx >= 0) ? ctx->slots[idx].offset : -1;
}

static const char *string_label_for(CodeGenCtx *ctx, const char *value) {
    if (!value) return NULL;
    for (int i = 0; i < ctx->string_count; i++) {
        if (strcmp(ctx->strings[i].value, value) == 0) return ctx->strings[i].label;
    }
    ensure_string_cap(ctx);
    StringLiteral *lit = &ctx->strings[ctx->string_count++];
    lit->value = (char *)value;
    snprintf(lit->label, sizeof(lit->label), ".LC%d", ctx->string_count - 1);
    return lit->label;
}

static void collect_operand_literals(CodeGenCtx *ctx, const Operand *op) {
    if (!op) return;
    if (op->kind == OPERAND_STRING && op->str_val) {
        (void)string_label_for(ctx, op->str_val);
    }
}

static void collect_function_layout(CodeGenCtx *ctx, TACFunc *func) {
    ctx->slot_count = 0;
    int next_offset = 4;
    for (int i = 0; i < func->instr_count; i++) {
        TACInstr *ins = func->instrs[i];
        add_slot_for_operand(ctx, &ins->dest, &next_offset);
        add_slot_for_operand(ctx, &ins->left, &next_offset);
        add_slot_for_operand(ctx, &ins->right, &next_offset);
        collect_operand_literals(ctx, &ins->dest);
        collect_operand_literals(ctx, &ins->left);
        collect_operand_literals(ctx, &ins->right);
    }
}

static int current_stack_size(CodeGenCtx *ctx) {
    int bytes = ctx->slot_count * 4;
    if (bytes == 0) bytes = 16;
    return align16(bytes);
}

static void emit_escaped_asciz(FILE *out, const char *s) {
    size_t start = 0;
    size_t end = s ? strlen(s) : 0;
    if (s && end >= 2 && s[0] == '"' && s[end - 1] == '"') {
        start = 1;
        end -= 1;
    }

    fputc('"', out);
    for (size_t i = start; i < end; i++) {
        unsigned char c = (unsigned char)s[i];
        switch (c) {
        case '\\': fputs("\\\\", out); break;
        case '"': fputs("\\\"", out); break;
        case '\n': fputs("\\n", out); break;
        case '\t': fputs("\\t", out); break;
        case '\r': fputs("\\r", out); break;
        default:
            if (isprint(c)) fputc((int)c, out);
            else fprintf(out, "\\%03o", c);
        }
    }
    fputc('"', out);
}

static void operand_mem_ref(CodeGenCtx *ctx, const Operand *op, char *buf, size_t n) {
    int off = get_stack_offset(ctx, op);
    if (off < 0) snprintf(buf, n, "-4(%%rbp)");
    else snprintf(buf, n, "-%d(%%rbp)", off);
}

static void emit_load_i32(CodeGenCtx *ctx, const Operand *op, const char *reg32) {
    char mem[64];
    if (!op) return;
    switch (op->kind) {
    case OPERAND_CONST_INT:
    case OPERAND_CONST_BOOL:
    case OPERAND_CONST_CHAR:
        fprintf(ctx->out, "    movl $%d, %s\n", op->int_val, reg32);
        break;
    case OPERAND_VAR:
    case OPERAND_TEMP:
        operand_mem_ref(ctx, op, mem, sizeof(mem));
        fprintf(ctx->out, "    movl %s, %s\n", mem, reg32);
        break;
    default:
        fprintf(ctx->out, "    movl $0, %s\n", reg32);
    }
}

static void emit_store_i32(CodeGenCtx *ctx, const char *reg32, const Operand *dst) {
    char mem[64];
    if (!dst) return;
    if (dst->kind == OPERAND_VAR || dst->kind == OPERAND_TEMP) {
        operand_mem_ref(ctx, dst, mem, sizeof(mem));
        fprintf(ctx->out, "    movl %s, %s\n", reg32, mem);
    }
}

static void gen_function_prologue(CodeGenCtx *ctx, const char *func_name, int stack_size) {
    fprintf(ctx->out, ".globl %s\n", func_name);
    fprintf(ctx->out, ".type %s, @function\n", func_name);
    fprintf(ctx->out, "%s:\n", func_name);
    fprintf(ctx->out, "    push %%rbp\n");
    fprintf(ctx->out, "    mov %%rsp, %%rbp\n");
    fprintf(ctx->out, "    sub $%d, %%rsp\n", stack_size);
}

static void gen_function_epilogue(CodeGenCtx *ctx) {
    fprintf(ctx->out, "    leave\n");
    fprintf(ctx->out, "    ret\n\n");
}

static void flush_pending_params(CodeGenCtx *ctx) {
    static const char *arg_regs32[] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    static const char *arg_regs64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int n = ctx->pending_param_count;
    if (n > 6) n = 6;

    for (int i = 0; i < n; i++) {
        Operand *arg = &ctx->pending_params[i];
        if (arg->kind == OPERAND_STRING && arg->str_val) {
            const char *lbl = string_label_for(ctx, arg->str_val);
            fprintf(ctx->out, "    leaq %s(%%rip), %s\n", lbl, arg_regs64[i]);
        } else {
            emit_load_i32(ctx, arg, arg_regs32[i]);
        }
    }
    ctx->pending_param_count = 0;
}

static void gen_instr(CodeGenCtx *ctx, TACInstr *instr) {
    if (!instr) return;

    switch (instr->op) {
    case TAC_ASSIGN:
    case TAC_ASSIGN_CONST:
        emit_load_i32(ctx, &instr->left, "%eax");
        emit_store_i32(ctx, "%eax", &instr->dest);
        break;

    case TAC_BINOP: {
        const char *op = instr->attr ? instr->attr : "";
        emit_load_i32(ctx, &instr->left, "%eax");

        if (strcmp(op, "+") == 0) {
            emit_load_i32(ctx, &instr->right, "%ecx");
            fprintf(ctx->out, "    addl %%ecx, %%eax\n");
        } else if (strcmp(op, "-") == 0) {
            emit_load_i32(ctx, &instr->right, "%ecx");
            fprintf(ctx->out, "    subl %%ecx, %%eax\n");
        } else if (strcmp(op, "*") == 0) {
            emit_load_i32(ctx, &instr->right, "%ecx");
            fprintf(ctx->out, "    imull %%ecx, %%eax\n");
        } else if (strcmp(op, "/") == 0) {
            emit_load_i32(ctx, &instr->right, "%ecx");
            fprintf(ctx->out, "    cltd\n");
            fprintf(ctx->out, "    idivl %%ecx\n");
        } else if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
                   strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0 ||
                   strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
            emit_load_i32(ctx, &instr->right, "%ecx");
            fprintf(ctx->out, "    cmpl %%ecx, %%eax\n");
            if (strcmp(op, "<") == 0) fprintf(ctx->out, "    setl %%al\n");
            else if (strcmp(op, ">") == 0) fprintf(ctx->out, "    setg %%al\n");
            else if (strcmp(op, "<=") == 0) fprintf(ctx->out, "    setle %%al\n");
            else if (strcmp(op, ">=") == 0) fprintf(ctx->out, "    setge %%al\n");
            else if (strcmp(op, "==") == 0) fprintf(ctx->out, "    sete %%al\n");
            else fprintf(ctx->out, "    setne %%al\n");
            fprintf(ctx->out, "    movzbl %%al, %%eax\n");
        }
        emit_store_i32(ctx, "%eax", &instr->dest);
        break;
    }

    case TAC_LABEL:
        if (instr->dest.kind == OPERAND_LABEL && instr->dest.name) {
            fprintf(ctx->out, "%s:\n", instr->dest.name);
        }
        break;

    case TAC_JUMP:
        if (instr->dest.kind == OPERAND_LABEL && instr->dest.name) {
            fprintf(ctx->out, "    jmp %s\n", instr->dest.name);
        }
        break;

    case TAC_COND_JUMP:
        emit_load_i32(ctx, &instr->left, "%eax");
        fprintf(ctx->out, "    cmpl $0, %%eax\n");
        if (instr->dest.kind == OPERAND_LABEL && instr->dest.name) {
            fprintf(ctx->out, "    jne %s\n", instr->dest.name);
        }
        break;

    case TAC_PARAM:
        if (ctx->pending_param_count < (int)(sizeof(ctx->pending_params) / sizeof(ctx->pending_params[0]))) {
            ctx->pending_params[ctx->pending_param_count++] = instr->left;
        }
        break;

    case TAC_CALL:
        flush_pending_params(ctx);
        if (instr->attr && strcmp(instr->attr, "printf") == 0) {
            fprintf(ctx->out, "    movl $0, %%eax\n");
        }
        if (instr->attr) {
            fprintf(ctx->out, "    call %s\n", instr->attr);
        }
        emit_store_i32(ctx, "%eax", &instr->dest);
        break;

    case TAC_RETURN:
        if (instr->left.kind != OPERAND_INVALID) {
            emit_load_i32(ctx, &instr->left, "%eax");
        }
        fprintf(ctx->out, "    jmp %s\n", ctx->return_label);
        break;

    case TAC_VAR:
        break;

    default:
        break;
    }
}

static void gen_function(CodeGenCtx *ctx, TACFunc *func) {
    if (!func) return;

    ctx->current_func = func;
    ctx->pending_param_count = 0;
    collect_function_layout(ctx, func);
    snprintf(ctx->return_label, sizeof(ctx->return_label), ".L_return_%s", func->name);

    gen_function_prologue(ctx, func->name, current_stack_size(ctx));
    for (int i = 0; i < func->instr_count; i++) {
        gen_instr(ctx, func->instrs[i]);
    }
    fprintf(ctx->out, "%s:\n", ctx->return_label);
    gen_function_epilogue(ctx);
}

void tac_codegen_x86(TACProgram *prog, FILE *out) {
    if (!prog || !out) return;

    CodeGenCtx ctx = {0};
    ctx.out = out;
    ctx.prog = prog;

    fprintf(out, ".section .text\n");
    fprintf(out, ".align 4\n\n");
    for (int i = 0; i < prog->func_count; i++) {
        gen_function(&ctx, prog->funcs[i]);
    }

    fprintf(out, ".section .rodata\n");
    fprintf(out, ".align 4\n");
    for (int i = 0; i < ctx.string_count; i++) {
        fprintf(out, "%s:\n", ctx.strings[i].label);
        fprintf(out, "    .asciz ");
        emit_escaped_asciz(out, ctx.strings[i].value ? ctx.strings[i].value : "");
        fprintf(out, "\n");
    }
    fprintf(out, "\n");

    free(ctx.slots);
    free(ctx.strings);
}
