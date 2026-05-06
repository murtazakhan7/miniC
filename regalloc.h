/*
 * regalloc.h — Register allocation interface
 */

#ifndef REGALLOC_H
#define REGALLOC_H

#include "backend.h"

typedef struct {
    char *temp_name;
    int first_use;
    int last_use;
    int allocated_reg;
    int spill_slot;
} LiveRange;

typedef struct {
    LiveRange *ranges;
    int range_count;
} RegAllocResult;

/* Perform linear-scan register allocation on a function */
RegAllocResult *regalloc_linear_scan(TACFunc *func);

void regalloc_free(RegAllocResult *result);

/* Query allocated register for a temporary */
const char *regalloc_get_register(RegAllocResult *result, const char *temp_name);

/* Get spill slot offset */
int regalloc_get_spill_offset(RegAllocResult *result, const char *temp_name);

#endif
