# Makefile — MiniC Compiler
#
# Requirements: flex, bison, gcc
#
# All generated artifacts go under $(BUILD_DIR)/ (objects, flex/bison output, binary).
#
# Targets:
#   make          — build $(BUILD_DIR)/minic
#   make test     — run all tests/*.c through the compiler
#   make tokens   — token stream for tests/test1.c
#   make ast      — AST for tests/test1.c
#   make clean    — remove $(BUILD_DIR)/

CC      = gcc
CFLAGS  = -Wall -Wextra -g
LEX     = flex
BISON   = bison

BUILD_DIR = build

MINIC     = $(BUILD_DIR)/minic

# Hand-written sources (project root)
HAND_SRCS = ast.c semantic.c ir.c backend.c optimizer.c regalloc.c codegen.c main.c
HAND_OBJS = $(HAND_SRCS:%.c=$(BUILD_DIR)/%.o)

# Flex / Bison outputs live under build/
BISON_TAB_C  = $(BUILD_DIR)/parser.tab.c
BISON_TAB_H  = $(BUILD_DIR)/parser.tab.h
LEX_GEN_C    = $(BUILD_DIR)/lex.yy.c
GEN_OBJS     = $(BUILD_DIR)/parser.tab.o $(BUILD_DIR)/lex.yy.o

OBJS = $(HAND_OBJS) $(GEN_OBJS)

# ── Default ────────────────────────────────────────────────────────────────
all: $(MINIC)

$(MINIC): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ── Code generation (Bison must run before Flex) ───────────────────────────
# Grouped targets (&:) so Bison runs once for both .c and .h (GNU Make 4.3+).
$(BISON_TAB_C) $(BISON_TAB_H) &: parser.y | $(BUILD_DIR)
	$(BISON) -d -v -o $(BISON_TAB_C) parser.y
	@mv -f parser.output $(BUILD_DIR)/parser.output 2>/dev/null || true

$(LEX_GEN_C): lexer.l $(BISON_TAB_H) | $(BUILD_DIR)
	$(LEX) -o $@ lexer.l

# ── Objects from hand-written sources ──────────────────────────────────────
$(BUILD_DIR)/%.o: %.c $(BISON_TAB_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -I. -c -o $@ $<

$(BUILD_DIR)/parser.tab.o: $(BISON_TAB_C) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -I. -c -o $@ $<

$(BUILD_DIR)/lex.yy.o: $(LEX_GEN_C) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -I. -Wno-unused-function -c -o $@ $<

# ── Test helpers ──────────────────────────────────────────────────────────
test: $(MINIC)
	@echo ""
	@for f in tests/*.c; do \
	    echo "=== Testing $$f ==="; \
	    $(MINIC) $$f && echo "PASS: $$f" || echo "FAIL: $$f"; \
	    echo ""; \
	done

tokens: $(MINIC)
	$(MINIC) --tokens tests/test1.c

ast: $(MINIC)
	$(MINIC) --ast tests/test1.c

# ── Clean ─────────────────────────────────────────────────────────────────
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test tokens ast clean
