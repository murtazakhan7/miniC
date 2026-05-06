# Makefile — MiniC Compiler (Member 1: Lexer + Parser)
#
# Requirements: flex, bison, gcc
#
# Targets:
#   make          — build ./minic
#   make test     — run all tests in tests/
#   make tokens   — show token stream for tests/test1.c
#   make ast      — show AST for tests/test1.c
#   make clean    — remove generated files

CC      = gcc
CFLAGS  = -Wall -Wextra -g
LEX     = flex
BISON   = bison

# Generated sources
BISON_SRC = parser.tab.c
BISON_HDR = parser.tab.h
LEX_SRC   = lex.yy.c

SRCS = $(BISON_SRC) $(LEX_SRC) ast.c semantic.c ir.c backend.c optimizer.c regalloc.c codegen.c main.c
OBJS = $(SRCS:.c=.o)

TARGET = minic

# ── Default ────────────────────────────────────────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ── Code generation ────────────────────────────────────────────────────────
$(BISON_SRC) $(BISON_HDR): parser.y
	$(BISON) -d -v parser.y

$(LEX_SRC): lexer.l $(BISON_HDR)
	$(LEX) lexer.l

# ── Compilation ────────────────────────────────────────────────────────────
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c -o $@ $<

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -Wno-unused-function -c -o $@ $<

# ── Test helpers ───────────────────────────────────────────────────────────
test: $(TARGET)
	@echo ""
	@for f in tests/*.c; do \
	    echo "=== Testing $$f ==="; \
	    ./$(TARGET) $$f && echo "PASS: $$f" || echo "FAIL: $$f"; \
	    echo ""; \
	done

tokens: $(TARGET)
	./$(TARGET) --tokens tests/test1.c

ast: $(TARGET)
	./$(TARGET) --ast tests/test1.c

# ── Clean ──────────────────────────────────────────────────────────────────
clean:
	rm -f $(TARGET) $(OBJS) $(BISON_SRC) $(BISON_HDR) $(LEX_SRC) \
	      parser.output *.o

.PHONY: all test tokens ast clean
