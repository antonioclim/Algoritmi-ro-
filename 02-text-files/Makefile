# =============================================================================
# SĂPTĂMÂNA 02: FIȘIERE TEXT - Makefile
# Algoritmi și Tehnici de Programare (ATP)
# =============================================================================

# Compilator și flag-uri
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -pedantic
LDFLAGS = 

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution
BUILD_DIR = build

# Culori pentru output (ANSI escape codes)
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
NC = \033[0m

# Executabile
TARGETS = example1 exercise1 exercise2
SOLUTION_TARGETS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# Fișiere sursă
EXAMPLE1_SRC = $(SRC_DIR)/example1.c
EXERCISE1_SRC = $(SRC_DIR)/exercise1.c
EXERCISE2_SRC = $(SRC_DIR)/exercise2.c

# =============================================================================
# REGULI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions run-example run-ex1 run-ex2

# Build all targets
all: $(TARGETS)
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║     ✓ Toate executabilele au fost compilate cu succes!        ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"

# Build example1
example1: $(EXAMPLE1_SRC)
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: example1.c                                          │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compilat cu succes$(NC)"

# Build exercise1
exercise1: $(EXERCISE1_SRC)
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: exercise1.c                                         │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 compilat cu succes$(NC)"

# Build exercise2
exercise2: $(EXERCISE2_SRC)
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: exercise2.c                                         │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 compilat cu succes$(NC)"

# =============================================================================
# REGULI DE RULARE
# =============================================================================

# Run all examples
run: all
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║     Rulare toate exemplele...                                 ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

# Run only example1
run-example: example1
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│  Rulare: example1                                               │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./example1

# Run exercise1
run-ex1: exercise1
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│  Rulare: exercise1 (Analizator Note)                            │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise1 $(DATA_DIR)/studgrades.csv

# Run exercise2
run-ex2: exercise2
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│  Rulare: exercise2 (Parser INI)                                 │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise2 $(DATA_DIR)/config_sample.ini

# =============================================================================
# REGULI DE TESTARE
# =============================================================================

# Run automated tests
test: exercise1 exercise2
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Rulare teste automate...                                  ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Verificare exercise1 cu date de test$(NC)"
	@if ./exercise1 $(DATA_DIR)/studgrades.csv > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 1 PASSED - exercise1 rulează fără erori$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 1 FAILED - exercise1 a întâmpinat erori$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Verificare exercise2 cu fișier INI$(NC)"
	@if ./exercise2 $(DATA_DIR)/config_sample.ini > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 2 PASSED - exercise2 rulează fără erori$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 2 FAILED - exercise2 a întâmpinat erori$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 3: Verificare generare raport$(NC)"
	@if [ -f report.txt ]; then \
		echo "$(GREEN)  ✓ Test 3 PASSED - raportul a fost generat$(NC)"; \
		rm -f report.txt; \
	else \
		echo "$(YELLOW)  ⚠ Test 3 SKIPPED - implementare incompletă$(NC)"; \
	fi

# =============================================================================
# VERIFICARE MEMORIE CU VALGRIND
# =============================================================================

valgrind: example1
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Verificare memory leaks cu Valgrind...                    ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@if command -v valgrind > /dev/null 2>&1; then \
		valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./example1; \
	else \
		echo "$(RED)  ✗ Valgrind nu este instalat$(NC)"; \
		echo "$(YELLOW)  Instalare: sudo apt install valgrind$(NC)"; \
	fi

valgrind-ex1: exercise1
	@echo "$(MAGENTA)Verificare memory leaks pentru exercise1...$(NC)"
	@valgrind --leak-check=full ./exercise1 $(DATA_DIR)/studgrades.csv

valgrind-ex2: exercise2
	@echo "$(MAGENTA)Verificare memory leaks pentru exercise2...$(NC)"
	@valgrind --leak-check=full ./exercise2 $(DATA_DIR)/config_sample.ini

# =============================================================================
# SOLUȚII (DOAR PENTRU INSTRUCTORI)
# =============================================================================

solutions: 
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Compilare soluții...                                      ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@if [ -f $(SOL_DIR)/exercise1_sol.c ]; then \
		$(CC) $(CFLAGS) -o exercise1_sol $(SOL_DIR)/exercise1_sol.c; \
		echo "$(GREEN)  ✓ exercise1_sol compilat$(NC)"; \
	else \
		echo "$(YELLOW)  ⚠ exercise1_sol.c nu există$(NC)"; \
	fi
	@if [ -f $(SOL_DIR)/exercise2_sol.c ]; then \
		$(CC) $(CFLAGS) -o exercise2_sol $(SOL_DIR)/exercise2_sol.c; \
		echo "$(GREEN)  ✓ exercise2_sol compilat$(NC)"; \
	else \
		echo "$(YELLOW)  ⚠ exercise2_sol.c nu există$(NC)"; \
	fi

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│  Curățare fișiere generate...                                   │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@rm -f $(TARGETS) $(SOLUTION_TARGETS)
	@rm -f *.o *.out
	@rm -f report.txt config_output.json
	@rm -f test_*.txt
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)  ✓ Curățare completă$(NC)"

# =============================================================================
# HELP
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║           SĂPTĂMÂNA 02: FIȘIERE TEXT - Makefile Help                  ║$(NC)"
	@echo "$(CYAN)╠═══════════════════════════════════════════════════════════════════════╣$(NC)"
	@echo "$(CYAN)║                                                                       ║$(NC)"
	@echo "$(CYAN)║  $(GREEN)Comenzi principale:$(CYAN)                                                ║$(NC)"
	@echo "$(CYAN)║    make              - Compilează toate executabilele                 ║$(NC)"
	@echo "$(CYAN)║    make run          - Rulează exemplul demonstrativ                  ║$(NC)"
	@echo "$(CYAN)║    make clean        - Șterge fișierele generate                      ║$(NC)"
	@echo "$(CYAN)║                                                                       ║$(NC)"
	@echo "$(CYAN)║  $(GREEN)Comenzi pentru exerciții:$(CYAN)                                          ║$(NC)"
	@echo "$(CYAN)║    make run-example  - Rulează example1 (demonstrație completă)       ║$(NC)"
	@echo "$(CYAN)║    make run-ex1      - Rulează exercise1 (analizator note)            ║$(NC)"
	@echo "$(CYAN)║    make run-ex2      - Rulează exercise2 (parser INI)                 ║$(NC)"
	@echo "$(CYAN)║                                                                       ║$(NC)"
	@echo "$(CYAN)║  $(GREEN)Comenzi pentru testare:$(CYAN)                                            ║$(NC)"
	@echo "$(CYAN)║    make test         - Rulează teste automate                         ║$(NC)"
	@echo "$(CYAN)║    make valgrind     - Verifică memory leaks (example1)               ║$(NC)"
	@echo "$(CYAN)║    make valgrind-ex1 - Verifică memory leaks (exercise1)              ║$(NC)"
	@echo "$(CYAN)║    make valgrind-ex2 - Verifică memory leaks (exercise2)              ║$(NC)"
	@echo "$(CYAN)║                                                                       ║$(NC)"
	@echo "$(CYAN)║  $(GREEN)Comenzi pentru instructori:$(CYAN)                                        ║$(NC)"
	@echo "$(CYAN)║    make solutions    - Compilează soluțiile                           ║$(NC)"
	@echo "$(CYAN)║                                                                       ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# INFORMAȚII SUPLIMENTARE
# =============================================================================

info:
	@echo "$(BLUE)Compilator: $(CC)$(NC)"
	@echo "$(BLUE)Flags: $(CFLAGS)$(NC)"
	@echo "$(BLUE)Surse: $(SRC_DIR)$(NC)"
	@echo "$(BLUE)Date: $(DATA_DIR)$(NC)"
	@echo "$(BLUE)Teste: $(TEST_DIR)$(NC)"

.DEFAULT_GOAL := help
