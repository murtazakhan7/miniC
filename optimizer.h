/*
 * optimizer.h — TAC optimization passes
 */

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "backend.h"

void tac_optimize(TACProgram *prog);
void tac_optimize_pass(TACProgram *prog, OptPassKind pass);

#endif
