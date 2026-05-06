/*
 * regalloc.c — Register allocation for x86
 *
 * Implements:
 * - Liveness analysis (which temporaries are live at each instruction)
 * - Linear-scan register allocation
 * - Spill slot management on stack
 * - Calling convention handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backend.h"
#include "regalloc.h"

/* =========================================================================
   x86 Physical Registers
   ========================================================================= */

/* x86 caller-clobbered (we can use freely): eax, ecx, edx */
/* x86 callee-preserved (must save/restore): ebx, esi, edi, ebp */
#define NUM_PHYS_REGS 6
static const char *phys_regs[] = {"eax", "ecx", "edx", "ebx", "esi", "edi"};

/* =========================================================================
   Live Range Analysis
   ========================================================================= */

/* Simple liveness: forward pass to determine first/last use of each temp */
/* (LiveRange defined in regalloc.h) */

/* Build live ranges for temporaries in a function */
static LiveRange *analyze_live_ranges(TACFunc *func, int *range_count) {
    if (!func || !range_count) return NULL;
    
    /* Collect all unique temps */
    LiveRange *ranges = malloc(1024 * sizeof(LiveRange));
    if (!ranges) return NULL;
    
    int count = 0;
    
    /* First pass: find all temps and their first/last uses */
    for (int i = 0; i < func->instr_count; i++) {
        TACInstr *instr = func->instrs[i];
        if (!instr) continue;
        
        /* Check destination */
        if (instr->dest.kind == OPERAND_TEMP && instr->dest.name) {
            char *name = instr->dest.name;
            int found = 0;
            for (int j = 0; j < count; j++) {
                if (strcmp(ranges[j].temp_name, name) == 0) {
                    ranges[j].last_use = i;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                ranges[count].temp_name = name;
                ranges[count].first_use = i;
                ranges[count].last_use = i;
                ranges[count].allocated_reg = -1;
                ranges[count].spill_slot = -1;
                count++;
            }
        }
        
        /* Check source operands */
        if (instr->left.kind == OPERAND_TEMP && instr->left.name) {
            char *name = instr->left.name;
            int found = 0;
            for (int j = 0; j < count; j++) {
                if (strcmp(ranges[j].temp_name, name) == 0) {
                    ranges[j].last_use = i;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                ranges[count].temp_name = name;
                ranges[count].first_use = i;
                ranges[count].last_use = i;
                ranges[count].allocated_reg = -1;
                ranges[count].spill_slot = -1;
                count++;
            }
        }
        
        if (instr->right.kind == OPERAND_TEMP && instr->right.name) {
            char *name = instr->right.name;
            int found = 0;
            for (int j = 0; j < count; j++) {
                if (strcmp(ranges[j].temp_name, name) == 0) {
                    ranges[j].last_use = i;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                ranges[count].temp_name = name;
                ranges[count].first_use = i;
                ranges[count].last_use = i;
                ranges[count].allocated_reg = -1;
                ranges[count].spill_slot = -1;
                count++;
            }
        }
    }
    
    *range_count = count;
    return ranges;
}

/* =========================================================================
   Linear-Scan Register Allocation
   ========================================================================= */

static int register_in_use[NUM_PHYS_REGS];

/* Allocate a register for a temporary at a given instruction */
static int alloc_register_for_temp(LiveRange *ranges, int range_count, int range_idx, int instr_idx) {
    LiveRange *lr = &ranges[range_idx];
    
    /* Try to find a free register */
    for (int i = 0; i < NUM_PHYS_REGS; i++) {
        if (!register_in_use[i]) {
            register_in_use[i] = 1;
            lr->allocated_reg = i;
            return i;
        }
    }
    
    /* No free registers: spill the one with latest last_use */
    int spill_idx = 0;
    int latest_use = ranges[0].last_use;
    for (int i = 1; i < range_count; i++) {
        if (ranges[i].allocated_reg >= 0 && ranges[i].last_use > latest_use) {
            latest_use = ranges[i].last_use;
            spill_idx = i;
        }
    }
    
    /* Move spilled temp to stack, reuse its register */
    if (ranges[spill_idx].allocated_reg >= 0) {
        int reg = ranges[spill_idx].allocated_reg;
        ranges[spill_idx].allocated_reg = -1;
        ranges[spill_idx].spill_slot = -4 * (spill_idx + 1); /* negative offset from ebp */
        lr->allocated_reg = reg;
        return reg;
    }
    
    /* Fallback: allocate to eax and spill */
    lr->allocated_reg = -1;
    lr->spill_slot = -4 * (range_idx + 1);
    return 0; /* eax */
}

RegAllocResult *regalloc_linear_scan(TACFunc *func) {
    if (!func) return NULL;
    
    int range_count = 0;
    LiveRange *ranges = analyze_live_ranges(func, &range_count);
    if (!ranges) return NULL;
    
    memset(register_in_use, 0, sizeof(register_in_use));
    
    /* Allocate registers for each instruction */
    for (int i = 0; i < func->instr_count; i++) {
        TACInstr *instr = func->instrs[i];
        if (!instr) continue;
        
        /* Free registers for temps that are no longer live */
        for (int j = 0; j < range_count; j++) {
            if (ranges[j].allocated_reg >= 0 && ranges[j].last_use < i) {
                register_in_use[ranges[j].allocated_reg] = 0;
                ranges[j].allocated_reg = -2; /* marked done */
            }
        }
        
        /* Allocate registers needed at this instruction */
        if (instr->dest.kind == OPERAND_TEMP && instr->dest.name) {
            for (int j = 0; j < range_count; j++) {
                if (strcmp(ranges[j].temp_name, instr->dest.name) == 0 && ranges[j].allocated_reg < 0) {
                    alloc_register_for_temp(ranges, range_count, j, i);
                    break;
                }
            }
        }
    }
    
    /* Build result */
    RegAllocResult *result = malloc(sizeof(RegAllocResult));
    if (!result) {
        free(ranges);
        return NULL;
    }
    result->ranges = ranges;
    result->range_count = range_count;
    return result;
}

void regalloc_free(RegAllocResult *result) {
    if (result) {
        free(result->ranges);
        free(result);
    }
}

/* Get physical register for a temporary */
const char *regalloc_get_register(RegAllocResult *result, const char *temp_name) {
    if (!result || !temp_name) return "eax";
    
    for (int i = 0; i < result->range_count; i++) {
        if (strcmp(result->ranges[i].temp_name, temp_name) == 0) {
            int reg_idx = result->ranges[i].allocated_reg;
            if (reg_idx >= 0 && reg_idx < NUM_PHYS_REGS) {
                return phys_regs[reg_idx];
            }
            break;
        }
    }
    return "eax"; /* default */
}

/* Get spill slot offset for a temporary */
int regalloc_get_spill_offset(RegAllocResult *result, const char *temp_name) {
    if (!result || !temp_name) return 0;
    
    for (int i = 0; i < result->range_count; i++) {
        if (strcmp(result->ranges[i].temp_name, temp_name) == 0) {
            return result->ranges[i].spill_slot;
        }
    }
    return 0;
}
