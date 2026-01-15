# =============================================================================
# SĂPTĂMÂNA 04: LISTE ÎNLĂNȚUITE (LINKED LISTS) - Makefile
# =============================================================================
# Curs: Algoritmi și Tehnici de Programare (ATP)
# Universitate: Academia de Studii Economice, București
# =============================================================================

# Compilator și opțiuni
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS =

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# Culori ANSI pentru output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
BOLD = \033[1m
NC = \033[0m

# Ținte principale
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# ȚINTE PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions check

all: $(TARGETS)
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║  $(BOLD)✓ Toate programele au fost compilate cu succes!$(NC)$(GREEN)              ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Programe disponibile:$(NC)"
	@echo "  $(YELLOW)./example1$(NC)   - Exemplu demonstrativ complet"
	@echo "  $(YELLOW)./exercise1$(NC)  - Exercițiu: Lista de studenți"
	@echo "  $(YELLOW)./exercise2$(NC)  - Exercițiu: Playlist dublu înlănțuit"
	@echo ""

# =============================================================================
# COMPILARE EXEMPLE ȘI EXERCIȚII
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)┌─────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│ Compilare: $(BOLD)example1.c$(NC)$(CYAN)                   │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ example1 compilat$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)┌─────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│ Compilare: $(BOLD)exercise1.c$(NC)$(CYAN)                  │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ exercise1 compilat$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)┌─────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│ Compilare: $(BOLD)exercise2.c$(NC)$(CYAN)                  │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ exercise2 compilat$(NC)"

# =============================================================================
# COMPILARE SOLUȚII (pentru instructori)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║  $(BOLD)✓ Toate soluțiile au fost compilate!$(NC)$(GREEN)                        ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)  Compilare soluție: exercise1_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ exercise1_sol compilat$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)  Compilare soluție: exercise2_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ exercise2_sol compilat$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)  Compilare soluție: homework1_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ homework1_sol compilat$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)  Compilare soluție: homework2_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ homework2_sol compilat$(NC)"

# =============================================================================
# EXECUTARE
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  $(BOLD)Executare: example1$(NC)$(YELLOW)                                         ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  $(BOLD)Executare: exercise1$(NC)$(YELLOW)                                        ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise1 < $(DATA_DIR)/studenti.txt

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  $(BOLD)Executare: exercise2$(NC)$(YELLOW)                                        ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise2 < $(DATA_DIR)/playlist.txt

# =============================================================================
# TESTARE AUTOMATĂ
# =============================================================================

test: exercise1 exercise2
	@echo ""
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║  $(BOLD)Rulare teste automate$(NC)$(BLUE)                                        ║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Lista de studenți$(NC)"
	@./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1 || true
	@if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 1 PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 1 FAILED$(NC)"; \
		echo "$(YELLOW)  Diferențe:$(NC)"; \
		diff /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt || true; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Playlist dublu înlănțuit$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1 || true
	@if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 2 PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 2 FAILED$(NC)"; \
		echo "$(YELLOW)  Diferențe:$(NC)"; \
		diff /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt || true; \
	fi
	@echo ""

# =============================================================================
# VERIFICARE MEMORIE CU VALGRIND
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║  $(BOLD)Verificare memorie cu Valgrind$(NC)$(MAGENTA)                            ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1

valgrind-ex1: exercise1
	@echo "$(MAGENTA)Verificare memorie: exercise1$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise1 < $(DATA_DIR)/studenti.txt

valgrind-ex2: exercise2
	@echo "$(MAGENTA)Verificare memorie: exercise2$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise2 < $(DATA_DIR)/playlist.txt

# =============================================================================
# VERIFICARE STATICĂ A CODULUI
# =============================================================================

check:
	@echo ""
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║  $(BOLD)Verificare statică a codului$(NC)$(BLUE)                                 ║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Verificare cu cppcheck (dacă este instalat):$(NC)"
	@which cppcheck > /dev/null 2>&1 && cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_DIR)/*.c || echo "  cppcheck nu este instalat"
	@echo ""

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  $(BOLD)Curățare fișiere generate$(NC)$(YELLOW)                                  ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS) *.o
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)  ✓ Curățare completă$(NC)"
	@echo ""

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)║     SĂPTĂMÂNA 04: LISTE ÎNLĂNȚUITE - Comenzi Disponibile      ║$(NC)"
	@echo "$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Compilare:$(NC)"
	@echo "  $(YELLOW)make$(NC)            - Compilează toate programele"
	@echo "  $(YELLOW)make example1$(NC)   - Compilează doar exemplul demonstrativ"
	@echo "  $(YELLOW)make exercise1$(NC)  - Compilează exercițiul 1"
	@echo "  $(YELLOW)make exercise2$(NC)  - Compilează exercițiul 2"
	@echo "  $(YELLOW)make solutions$(NC)  - Compilează soluțiile (instructor)"
	@echo ""
	@echo "$(CYAN)Executare:$(NC)"
	@echo "  $(YELLOW)make run$(NC)        - Rulează exemplul demonstrativ"
	@echo "  $(YELLOW)make run-ex1$(NC)    - Rulează exercițiul 1 cu date de test"
	@echo "  $(YELLOW)make run-ex2$(NC)    - Rulează exercițiul 2 cu date de test"
	@echo ""
	@echo "$(CYAN)Testare:$(NC)"
	@echo "  $(YELLOW)make test$(NC)       - Rulează testele automate"
	@echo "  $(YELLOW)make valgrind$(NC)   - Verifică memory leaks în example1"
	@echo "  $(YELLOW)make check$(NC)      - Analiză statică cu cppcheck"
	@echo ""
	@echo "$(CYAN)Altele:$(NC)"
	@echo "  $(YELLOW)make clean$(NC)      - Șterge fișierele compilate"
	@echo "  $(YELLOW)make help$(NC)       - Afișează acest mesaj"
	@echo ""
	@echo "$(BOLD)Fișiere de date:$(NC)"
	@echo "  $(DATA_DIR)/studenti.txt  - Date pentru exercițiul 1"
	@echo "  $(DATA_DIR)/playlist.txt  - Date pentru exercițiul 2"
	@echo ""

# =============================================================================
# SFÂRȘITUL MAKEFILE
# =============================================================================
