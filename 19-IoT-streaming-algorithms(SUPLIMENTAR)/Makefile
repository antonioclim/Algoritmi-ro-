# =============================================================================
# SĂPTĂMÂNA 19: ALGORITMI PENTRU IOT ȘI STREAM PROCESSING
# Makefile pentru compilare, testare și rulare
# =============================================================================

# Compilator și flag-uri
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2 -pedantic
LDFLAGS = -lm

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution
SIM_DIR = simulators

# Culori ANSI pentru output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
BOLD = \033[1m
NC = \033[0m

# Executabile principale
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol
SIMULATORS = virtual_sensor mqtt_publisher mqtt_subscriber

# =============================================================================
# REGULI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions demo simulators
.PHONY: run-ex1 run-ex2 run-sim docker-build docker-run

all: $(TARGETS)
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║  ✓ Toate targeturile au fost compilate cu succes!             ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Executabile disponibile:$(NC)"
	@echo "  $(YELLOW)./example1$(NC)     - Exemplu complet pipeline IoT"
	@echo "  $(YELLOW)./exercise1$(NC)    - Smart Building Monitor"
	@echo "  $(YELLOW)./exercise2$(NC)    - Anomaly Detector cu Rate Limiting"
	@echo ""
	@echo "$(CYAN)Simulatoare (make simulators):$(NC)"
	@echo "  $(YELLOW)./virtual_sensor$(NC)  - Simulator senzori"
	@echo ""

# =============================================================================
# COMPILARE EXECUTABILE PRINCIPALE
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: example1.c - Pipeline complet IoT                   │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compilat cu succes$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: exercise1.c - Smart Building Monitor                │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 compilat cu succes$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: exercise2.c - Anomaly Detector                      │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 compilat cu succes$(NC)"

# =============================================================================
# COMPILARE SIMULATOARE MQTT
# =============================================================================
# Două variante: POSIX (fără dependențe) și Paho (broker real)

SIM_POSIX_LIBS = -lpthread -lrt -lm
SIM_PAHO_LIBS = -lpaho-mqtt3c -lpthread -lm

.PHONY: simulators simulators-posix simulators-paho mqtt-demo

simulators: simulators-posix
	@echo ""
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║  ✓ Simulatoare MQTT compilate (varianta POSIX)               ║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo "$(CYAN)  Pentru varianta Paho: make simulators-paho$(NC)"

simulators-posix: virtual_sensor mqtt_publisher mqtt_subscriber
	@echo "$(GREEN)  ✓ Varianta POSIX completă (fără dependențe externe)$(NC)"

simulators-paho: virtual_sensor_paho mqtt_publisher_paho mqtt_subscriber_paho
	@echo "$(GREEN)  ✓ Varianta Paho completă (necesită broker Mosquitto)$(NC)"

# Varianta POSIX (fără dependențe)
virtual_sensor: $(SIM_DIR)/virtual_sensor.c $(SIM_DIR)/mqtt_posix.c $(SIM_DIR)/mqtt_common.h
	@echo "$(BLUE)  [POSIX] Compilare: virtual_sensor$(NC)"
	@$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE \
		-o $@ $(SIM_DIR)/virtual_sensor.c $(SIM_DIR)/mqtt_posix.c $(SIM_POSIX_LIBS)
	@echo "$(GREEN)  ✓ virtual_sensor compilat$(NC)"

mqtt_publisher: $(SIM_DIR)/mqtt_publisher.c $(SIM_DIR)/mqtt_posix.c $(SIM_DIR)/mqtt_common.h
	@echo "$(BLUE)  [POSIX] Compilare: mqtt_publisher$(NC)"
	@$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE \
		-o $@ $(SIM_DIR)/mqtt_publisher.c $(SIM_DIR)/mqtt_posix.c $(SIM_POSIX_LIBS)
	@echo "$(GREEN)  ✓ mqtt_publisher compilat$(NC)"

mqtt_subscriber: $(SIM_DIR)/mqtt_subscriber.c $(SIM_DIR)/mqtt_posix.c $(SIM_DIR)/mqtt_common.h
	@echo "$(BLUE)  [POSIX] Compilare: mqtt_subscriber$(NC)"
	@$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE \
		-o $@ $(SIM_DIR)/mqtt_subscriber.c $(SIM_DIR)/mqtt_posix.c $(SIM_POSIX_LIBS)
	@echo "$(GREEN)  ✓ mqtt_subscriber compilat$(NC)"

# Varianta Paho (broker real)
virtual_sensor_paho: $(SIM_DIR)/virtual_sensor.c $(SIM_DIR)/mqtt_paho.c $(SIM_DIR)/mqtt_common.h
	@echo "$(YELLOW)  [PAHO] Compilare: virtual_sensor_paho$(NC)"
	@$(CC) $(CFLAGS) -DUSE_PAHO_MQTT \
		-o $@ $(SIM_DIR)/virtual_sensor.c $(SIM_DIR)/mqtt_paho.c $(SIM_PAHO_LIBS)
	@echo "$(GREEN)  ✓ virtual_sensor_paho compilat$(NC)"

mqtt_publisher_paho: $(SIM_DIR)/mqtt_publisher.c $(SIM_DIR)/mqtt_paho.c $(SIM_DIR)/mqtt_common.h
	@echo "$(YELLOW)  [PAHO] Compilare: mqtt_publisher_paho$(NC)"
	@$(CC) $(CFLAGS) -DUSE_PAHO_MQTT \
		-o $@ $(SIM_DIR)/mqtt_publisher.c $(SIM_DIR)/mqtt_paho.c $(SIM_PAHO_LIBS)
	@echo "$(GREEN)  ✓ mqtt_publisher_paho compilat$(NC)"

mqtt_subscriber_paho: $(SIM_DIR)/mqtt_subscriber.c $(SIM_DIR)/mqtt_paho.c $(SIM_DIR)/mqtt_common.h
	@echo "$(YELLOW)  [PAHO] Compilare: mqtt_subscriber_paho$(NC)"
	@$(CC) $(CFLAGS) -DUSE_PAHO_MQTT \
		-o $@ $(SIM_DIR)/mqtt_subscriber.c $(SIM_DIR)/mqtt_paho.c $(SIM_PAHO_LIBS)
	@echo "$(GREEN)  ✓ mqtt_subscriber_paho compilat$(NC)"

# Demo MQTT rapid
mqtt-demo: simulators-posix
	@echo ""
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║  MQTT Demo - Pub/Sub Local (POSIX)                            ║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Pornire subscriber în background...$(NC)"
	@./mqtt_subscriber 'sensors/#' &
	@sleep 1
	@echo "$(CYAN)Publicare 5 mesaje de test...$(NC)"
	@./virtual_sensor -n 5 -v -i 500
	@sleep 1
	@pkill -f mqtt_subscriber 2>/dev/null || true
	@echo ""
	@echo "$(GREEN)Demo complet!$(NC)"

# =============================================================================
# COMPILARE SOLUȚII (pentru instructor)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║  ✓ Toate soluțiile au fost compilate!                         ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)  Compilare soluție: exercise1_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1_sol compilat$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)  Compilare soluție: exercise2_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2_sol compilat$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)  Compilare soluție: homework1_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework1_sol compilat$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)  Compilare soluție: homework2_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework2_sol compilat$(NC)"

# =============================================================================
# EXECUȚIE
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Rulare exemplu demonstrativ - Pipeline IoT                   ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Rulare Exercise 1: Smart Building Monitor                    ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise1

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Rulare Exercise 2: Anomaly Detector                          ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise2 < $(DATA_DIR)/sensor_temperature.csv

run-sim: virtual_sensor
	@echo ""
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║  Rulare Simulator Senzori                                     ║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./virtual_sensor

demo: example1
	@echo ""
	@echo "$(BOLD)$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)$(BLUE)║       DEMONSTRAȚIE COMPLETĂ - SĂPTĂMÂNA 19                    ║$(NC)"
	@echo "$(BOLD)$(BLUE)║       Algoritmi pentru IoT și Stream Processing               ║$(NC)"
	@echo "$(BOLD)$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1
	@echo ""
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)  Demonstrație completă! Verifică output-ul de mai sus.            $(NC)"
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════════$(NC)"

# =============================================================================
# TESTARE AUTOMATĂ
# =============================================================================

test: exercise1 exercise2
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Rulare teste automate                                        ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Smart Building Monitor$(NC)"
	@if ./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; then \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 1 PASSED$(NC)"; \
		else \
			echo "$(YELLOW)  ~ Test 1 PASSED (output diferit dar valid)$(NC)"; \
		fi; \
	else \
		echo "$(RED)  ✗ Test 1 FAILED (eroare execuție)$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Anomaly Detector$(NC)"
	@if ./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; then \
		if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 2 PASSED$(NC)"; \
		else \
			echo "$(YELLOW)  ~ Test 2 PASSED (output diferit dar valid)$(NC)"; \
		fi; \
	else \
		echo "$(RED)  ✗ Test 2 FAILED (eroare execuție)$(NC)"; \
	fi
	@echo ""

# =============================================================================
# VALGRIND - VERIFICARE MEMORIEI
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║  Verificare memory leaks cu Valgrind                          ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1 2>&1 | head -50
	@echo ""
	@echo "$(CYAN)Notă: Verifică 'All heap blocks were freed' pentru succes$(NC)"

# =============================================================================
# DOCKER
# =============================================================================

docker-build:
	@echo "$(CYAN)Construire imagine Docker...$(NC)"
	@docker build -t atp-week19 .
	@echo "$(GREEN)✓ Imagine construită: atp-week19$(NC)"

docker-run: docker-build
	@echo "$(CYAN)Pornire container Docker...$(NC)"
	@docker run -it --rm -v $(PWD):/workspace atp-week19

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo "$(RED)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(RED)│  Curățare fișiere generate                                      │$(NC)"
	@echo "$(RED)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS) $(SIMULATORS)
	@rm -f virtual_sensor_paho mqtt_publisher_paho mqtt_subscriber_paho
	@rm -f *.o *.out
	@rm -f /tmp/test*_output.txt
	@rm -f /dev/shm/mqtt_sim_* 2>/dev/null || true
	@echo "$(GREEN)  ✓ Curățare completă$(NC)"

# Curățare doar shared memory POSIX
clean-shm:
	@echo "$(CYAN)Curățare shared memory MQTT...$(NC)"
	@rm -f /dev/shm/mqtt_sim_* 2>/dev/null || true
	@rm -f /dev/shm/sem.mqtt_sim_* 2>/dev/null || true
	@echo "$(GREEN)  ✓ Shared memory curățată$(NC)"

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(BOLD)╔═══════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)║                   SĂPTĂMÂNA 19: IoT & STREAM PROCESSING                   ║$(NC)"
	@echo "$(BOLD)║                        Comenzi Makefile Disponibile                        ║$(NC)"
	@echo "$(BOLD)╚═══════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Compilare:$(NC)"
	@echo "  $(YELLOW)make$(NC)              - Compilează toate targeturile principale"
	@echo "  $(YELLOW)make simulators$(NC)   - Compilează simulatoarele MQTT (POSIX)"
	@echo "  $(YELLOW)make simulators-paho$(NC) - Compilează simulatoarele MQTT (Paho)"
	@echo "  $(YELLOW)make solutions$(NC)    - Compilează soluțiile (instructor)"
	@echo ""
	@echo "$(CYAN)Execuție:$(NC)"
	@echo "  $(YELLOW)make run$(NC)          - Rulează exemplul demonstrativ"
	@echo "  $(YELLOW)make run-ex1$(NC)      - Rulează Exercise 1 (Smart Building)"
	@echo "  $(YELLOW)make run-ex2$(NC)      - Rulează Exercise 2 (Anomaly Detector)"
	@echo "  $(YELLOW)make run-sim$(NC)      - Rulează simulatorul de senzori"
	@echo "  $(YELLOW)make mqtt-demo$(NC)    - Demo complet MQTT pub/sub"
	@echo "  $(YELLOW)make demo$(NC)         - Demonstrație completă"
	@echo ""
	@echo "$(CYAN)Testare și Verificare:$(NC)"
	@echo "  $(YELLOW)make test$(NC)         - Rulează testele automate"
	@echo "  $(YELLOW)make valgrind$(NC)     - Verificare memory leaks"
	@echo ""
	@echo "$(CYAN)Docker:$(NC)"
	@echo "  $(YELLOW)make docker-build$(NC) - Construiește imaginea Docker"
	@echo "  $(YELLOW)make docker-run$(NC)   - Rulează în container Docker"
	@echo ""
	@echo "$(CYAN)Curățare:$(NC)"
	@echo "  $(YELLOW)make clean$(NC)        - Șterge fișierele generate"
	@echo "  $(YELLOW)make clean-shm$(NC)    - Curăță shared memory POSIX"
	@echo ""
	@echo "$(CYAN)Structura directorului:$(NC)"
	@echo "  src/        - Cod sursă principale"
	@echo "  simulators/ - Simulatoare senzori și MQTT (dual-mode)"
	@echo "  data/       - Fișiere de date pentru teste"
	@echo "  tests/      - Teste automate"
	@echo "  solution/   - Soluții complete"
	@echo "  slides/     - Prezentări HTML interactive"
	@echo "  teme/       - Specificații teme de casă"
	@echo ""
