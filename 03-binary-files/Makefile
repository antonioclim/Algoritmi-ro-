# =============================================================================
# SĂPTĂMÂNA 03: FIȘIERE BINARE - Makefile
# =============================================================================
# 
# Utilizare:
#   make          - Compilează toate target-urile
#   make run      - Rulează exemplul demonstrativ
#   make test     - Execută testele automate
#   make valgrind - Verifică memory leaks
#   make clean    - Șterge fișierele generate
#   make help     - Afișează acest mesaj
#
# =============================================================================

# Compilator și flag-uri
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -O0
LDFLAGS = -lm

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
BOLD = \033[1m
NC = \033[0m

# Caractere speciale
CHECK = ✓
CROSS = ✗
ARROW = →

# Target-uri principale
TARGETS = example1 exercise1 exercise2

# Target-uri soluții (doar pentru instructori)
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# TARGET-URI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions run-example run-ex1 run-ex2

all: banner $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(BOLD)$(CHECK) Toate target-urile au fost compilate cu succes!$(NC)"
	@echo ""

banner:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║$(NC)     $(BOLD)SĂPTĂMÂNA 03: FIȘIERE BINARE$(NC)                              $(CYAN)║$(NC)"
	@echo "$(CYAN)║$(NC)     Algoritmi și Tehnici de Programare                        $(CYAN)║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# COMPILARE EXEMPLE ȘI EXERCIȚII
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)$(ARROW) Compilare$(NC) example1.c $(CYAN)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)$(CHECK)$(NC) example1 compilat"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)$(ARROW) Compilare$(NC) exercise1.c $(CYAN)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)$(CHECK)$(NC) exercise1 compilat"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)$(ARROW) Compilare$(NC) exercise2.c $(CYAN)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)$(CHECK)$(NC) exercise2 compilat"

# =============================================================================
# COMPILARE SOLUȚII (DOAR PENTRU INSTRUCTORI)
# =============================================================================

solutions: banner $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)$(CHECK) Toate soluțiile au fost compilate!$(NC)"
	@echo ""

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)$(ARROW) Compilare$(NC) exercise1_sol.c $(MAGENTA)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)$(CHECK)$(NC) exercise1_sol compilat"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)$(ARROW) Compilare$(NC) exercise2_sol.c $(MAGENTA)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)$(CHECK)$(NC) exercise2_sol compilat"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)$(ARROW) Compilare$(NC) homework1_sol.c $(MAGENTA)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)$(CHECK)$(NC) homework1_sol compilat"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)$(ARROW) Compilare$(NC) homework2_sol.c $(MAGENTA)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(GREEN)$(CHECK)$(NC) homework2_sol compilat"

# =============================================================================
# RULARE PROGRAME
# =============================================================================

run: run-example

run-example: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(NC)     $(BOLD)RULARE: Exemplu Demonstrativ$(NC)                              $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(NC)     $(BOLD)RULARE: Exercițiul 1 - Catalog Studenți$(NC)                   $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise1

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(NC)     $(BOLD)RULARE: Exercițiul 2 - Convertor BMP$(NC)                      $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise2

# =============================================================================
# TESTARE AUTOMATĂ
# =============================================================================

test: exercise1 exercise2
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(NC)     $(BOLD)TESTARE AUTOMATĂ$(NC)                                          $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)$(ARROW) Testare Exercițiu 1...$(NC)"
	@if [ -f $(TEST_DIR)/test1_input.txt ]; then \
		./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "  $(GREEN)$(CHECK)$(NC) Test 1: $(GREEN)PASSED$(NC)"; \
		else \
			echo "  $(RED)$(CROSS)$(NC) Test 1: $(RED)FAILED$(NC)"; \
			echo "  $(YELLOW)Diferențe:$(NC)"; \
			diff $(TEST_DIR)/test1_expected.txt /tmp/test1_output.txt || true; \
		fi; \
	else \
		echo "  $(YELLOW)!$(NC) Test 1: Fișier input lipsă"; \
	fi
	@echo ""
	@echo "$(CYAN)$(ARROW) Testare Exercițiu 2...$(NC)"
	@if [ -f $(TEST_DIR)/test2_input.txt ]; then \
		./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; \
		if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
			echo "  $(GREEN)$(CHECK)$(NC) Test 2: $(GREEN)PASSED$(NC)"; \
		else \
			echo "  $(RED)$(CROSS)$(NC) Test 2: $(RED)FAILED$(NC)"; \
		fi; \
	else \
		echo "  $(YELLOW)!$(NC) Test 2: Fișier input lipsă"; \
	fi
	@echo ""

# =============================================================================
# VERIFICARE MEMORY LEAKS
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(NC)     $(BOLD)VERIFICARE MEMORY LEAKS CU VALGRIND$(NC)                       $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@if command -v valgrind > /dev/null 2>&1; then \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1; \
	else \
		echo "$(RED)$(CROSS)$(NC) Valgrind nu este instalat."; \
		echo "  $(YELLOW)$(ARROW)$(NC) Instalați cu: $(CYAN)sudo apt install valgrind$(NC)"; \
	fi

valgrind-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)$(ARROW) Verificare memory leaks pentru Exercițiul 1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise1 2>&1 || true

valgrind-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)$(ARROW) Verificare memory leaks pentru Exercițiul 2...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise2 2>&1 || true

# =============================================================================
# VERIFICARE STATICĂ CU CPPCHECK (OPȚIONAL)
# =============================================================================

check:
	@echo ""
	@echo "$(YELLOW)$(ARROW) Analiză statică cu cppcheck...$(NC)"
	@if command -v cppcheck > /dev/null 2>&1; then \
		cppcheck --enable=all --std=c11 $(SRC_DIR)/*.c 2>&1; \
	else \
		echo "$(YELLOW)!$(NC) cppcheck nu este instalat."; \
		echo "  $(YELLOW)$(ARROW)$(NC) Instalați cu: $(CYAN)sudo apt install cppcheck$(NC)"; \
	fi

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)$(ARROW) Curățare fișiere generate...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o *.bin *.bmp
	@rm -f /tmp/test*_output.txt
	@rm -f core vgcore.*
	@echo "  $(GREEN)$(CHECK)$(NC) Curățare completă!"
	@echo ""

# =============================================================================
# HELP
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║$(NC)     $(BOLD)SĂPTĂMÂNA 03: FIȘIERE BINARE - HELP$(NC)                        $(CYAN)║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Target-uri disponibile:$(NC)"
	@echo ""
	@echo "  $(GREEN)make$(NC)              Compilează toate target-urile"
	@echo "  $(GREEN)make example1$(NC)     Compilează exemplul demonstrativ"
	@echo "  $(GREEN)make exercise1$(NC)    Compilează exercițiul 1 (Catalog Studenți)"
	@echo "  $(GREEN)make exercise2$(NC)    Compilează exercițiul 2 (Convertor BMP)"
	@echo ""
	@echo "  $(YELLOW)make run$(NC)          Rulează exemplul demonstrativ"
	@echo "  $(YELLOW)make run-example$(NC)  Rulează exemplul demonstrativ"
	@echo "  $(YELLOW)make run-ex1$(NC)      Rulează exercițiul 1"
	@echo "  $(YELLOW)make run-ex2$(NC)      Rulează exercițiul 2"
	@echo ""
	@echo "  $(CYAN)make test$(NC)         Execută testele automate"
	@echo "  $(CYAN)make valgrind$(NC)     Verifică memory leaks"
	@echo "  $(CYAN)make check$(NC)        Analiză statică cu cppcheck"
	@echo ""
	@echo "  $(MAGENTA)make solutions$(NC)    Compilează soluțiile (doar instructori)"
	@echo ""
	@echo "  $(RED)make clean$(NC)        Șterge fișierele generate"
	@echo "  $(GREEN)make help$(NC)         Afișează acest mesaj"
	@echo ""
	@echo "$(BOLD)Structura proiectului:$(NC)"
	@echo ""
	@echo "  src/            Cod sursă (exemple și exerciții)"
	@echo "  data/           Fișiere de date pentru testare"
	@echo "  tests/          Teste automate"
	@echo "  solution/       Soluții (doar pentru instructori)"
	@echo "  slides/         Prezentări HTML"
	@echo "  teme/           Cerințe teme de casă"
	@echo ""

# =============================================================================
# GENERARE DATE TEST
# =============================================================================

generate-test-data: example1
	@echo "$(YELLOW)$(ARROW) Generare date de test...$(NC)"
	@./example1 --generate-test-data
	@echo "  $(GREEN)$(CHECK)$(NC) Date de test generate în $(DATA_DIR)/"

# =============================================================================
# INFORMAȚII DESPRE SISTEM
# =============================================================================

info:
	@echo ""
	@echo "$(CYAN)$(ARROW) Informații despre sistem:$(NC)"
	@echo "  Compilator: $(shell $(CC) --version | head -1)"
	@echo "  Arhitectură: $(shell uname -m)"
	@echo "  Sistem: $(shell uname -s) $(shell uname -r)"
	@echo "  Endianness: $(shell echo -n I | od -to2 | head -1 | cut -f2 -d' ' | cut -c6)"
	@echo "  sizeof(int): $(shell echo 'int main(){return sizeof(int);}' | $(CC) -x c - -o /tmp/sizeof_test && /tmp/sizeof_test; echo $$?)"
	@echo ""
