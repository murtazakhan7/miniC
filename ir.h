#ifndef IR_H
#define IR_H

#include "ast.h"
#include "backend.h"

void ir_generate(ASTNode *root, FILE *out);

/* Exported TAC program built during ir_generate */
extern TACProgram *ir_tac_program;

#endif
