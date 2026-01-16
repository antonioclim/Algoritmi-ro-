# ==============================================================================
# MAKEFILE - Simulatoare MQTT pentru IoT
# ==============================================================================
#
# Săptămâna 19: Algoritmi pentru IoT și Stream Processing
# Curs: Algoritmi și Tehnici de Programare, ASE București
#
# UTILIZARE:
#   make posix          - Compilează toate cu backend POSIX (fără dependențe)
#   make paho           - Compilează toate cu Paho MQTT (necesită libpaho)
#   make all            - Compilează ambele variante
#   make run-demo       - Rulează demo pub/sub POSIX
#   make clean          - Șterge binare
#
# ==============================================================================

# Compilator și flags
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -std=c11 -O2 -g
CFLAGS += -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE

# Biblioteci
LIBS_POSIX := -lpthread -lrt -lm
LIBS_PAHO := -lpaho-mqtt3c -lpthread -lm

# Directoare
BUILD_DIR := build
POSIX_DIR := $(BUILD_DIR)/posix
PAHO_DIR := $(BUILD_DIR)/paho

# Surse
COMMON_HDR := mqtt_common.h
POSIX_SRC := mqtt_posix.c
PAHO_SRC := mqtt_paho.c

# Aplicații
APPS := virtual_sensor mqtt_publisher mqtt_subscriber

# Culori
COLOR_RESET := \033[0m
COLOR_GREEN := \033[32m
COLOR_YELLOW := \033[33m
COLOR_CYAN := \033[36m
COLOR_BOLD := \033[1m

# ==============================================================================
# TARGETS PRINCIPALE
# ==============================================================================

.PHONY: all posix paho clean help run-demo run-sensor run-pub run-sub check-paho

all: posix paho
	@echo ""
	@echo "$(COLOR_GREEN)$(COLOR_BOLD)✓ Toate variantele compilate cu succes!$(COLOR_RESET)"
	@echo "  Binare POSIX: $(POSIX_DIR)/"
	@echo "  Binare Paho:  $(PAHO_DIR)/"

# ------------------------------------------------------------------------------
# Varianta POSIX (fără dependențe externe)
# ------------------------------------------------------------------------------

posix: $(POSIX_DIR) $(addprefix $(POSIX_DIR)/,$(APPS))
	@echo ""
	@echo "$(COLOR_GREEN)✓ Varianta POSIX compilată (fără dependențe externe)$(COLOR_RESET)"

$(POSIX_DIR):
	@mkdir -p $(POSIX_DIR)

$(POSIX_DIR)/virtual_sensor: virtual_sensor.c $(POSIX_SRC) $(COMMON_HDR)
	@echo "$(COLOR_CYAN)[POSIX]$(COLOR_RESET) Compilare virtual_sensor..."
	$(CC) $(CFLAGS) -o $@ virtual_sensor.c $(POSIX_SRC) $(LIBS_POSIX)

$(POSIX_DIR)/mqtt_publisher: mqtt_publisher.c $(POSIX_SRC) $(COMMON_HDR)
	@echo "$(COLOR_CYAN)[POSIX]$(COLOR_RESET) Compilare mqtt_publisher..."
	$(CC) $(CFLAGS) -o $@ mqtt_publisher.c $(POSIX_SRC) $(LIBS_POSIX)

$(POSIX_DIR)/mqtt_subscriber: mqtt_subscriber.c $(POSIX_SRC) $(COMMON_HDR)
	@echo "$(COLOR_CYAN)[POSIX]$(COLOR_RESET) Compilare mqtt_subscriber..."
	$(CC) $(CFLAGS) -o $@ mqtt_subscriber.c $(POSIX_SRC) $(LIBS_POSIX)

# ------------------------------------------------------------------------------
# Varianta Paho MQTT (necesită libpaho-mqtt-dev)
# ------------------------------------------------------------------------------

paho: check-paho $(PAHO_DIR) $(addprefix $(PAHO_DIR)/,$(APPS))
	@echo ""
	@echo "$(COLOR_GREEN)✓ Varianta Paho compilată (broker real)$(COLOR_RESET)"

$(PAHO_DIR):
	@mkdir -p $(PAHO_DIR)

$(PAHO_DIR)/virtual_sensor: virtual_sensor.c $(PAHO_SRC) $(COMMON_HDR)
	@echo "$(COLOR_YELLOW)[PAHO]$(COLOR_RESET) Compilare virtual_sensor..."
	$(CC) $(CFLAGS) -DUSE_PAHO_MQTT -o $@ virtual_sensor.c $(PAHO_SRC) $(LIBS_PAHO)

$(PAHO_DIR)/mqtt_publisher: mqtt_publisher.c $(PAHO_SRC) $(COMMON_HDR)
	@echo "$(COLOR_YELLOW)[PAHO]$(COLOR_RESET) Compilare mqtt_publisher..."
	$(CC) $(CFLAGS) -DUSE_PAHO_MQTT -o $@ mqtt_publisher.c $(PAHO_SRC) $(LIBS_PAHO)

$(PAHO_DIR)/mqtt_subscriber: mqtt_subscriber.c $(PAHO_SRC) $(COMMON_HDR)
	@echo "$(COLOR_YELLOW)[PAHO]$(COLOR_RESET) Compilare mqtt_subscriber..."
	$(CC) $(CFLAGS) -DUSE_PAHO_MQTT -o $@ mqtt_subscriber.c $(PAHO_SRC) $(LIBS_PAHO)

# Verificare Paho instalat
check-paho:
	@if ! pkg-config --exists libpaho-mqtt3c 2>/dev/null && \
	    ! [ -f /usr/lib/libpaho-mqtt3c.so ] && \
	    ! [ -f /usr/local/lib/libpaho-mqtt3c.so ]; then \
		echo "$(COLOR_YELLOW)⚠ Paho MQTT C nu este instalat$(COLOR_RESET)"; \
		echo "  Instalare: sudo apt install libpaho-mqtt-dev"; \
		echo "  Sau din sursă: https://github.com/eclipse/paho.mqtt.c"; \
		echo ""; \
		echo "  Continuare cu warning..."; \
	fi

# ==============================================================================
# TARGETS DE RULARE
# ==============================================================================

# Demo complet: pornește subscriber, apoi publisher
run-demo: posix
	@echo ""
	@echo "$(COLOR_BOLD)═══════════════════════════════════════════════════════════$(COLOR_RESET)"
	@echo "$(COLOR_BOLD)           MQTT POSIX DEMO - Pub/Sub Local                 $(COLOR_RESET)"
	@echo "$(COLOR_BOLD)═══════════════════════════════════════════════════════════$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_CYAN)Pornire subscriber în background...$(COLOR_RESET)"
	@$(POSIX_DIR)/mqtt_subscriber -t sensors/# &
	@sleep 1
	@echo ""
	@echo "$(COLOR_CYAN)Pornire sensor simulator (5 mesaje)...$(COLOR_RESET)"
	@$(POSIX_DIR)/virtual_sensor -n 5 -v -i 500
	@echo ""
	@echo "$(COLOR_CYAN)Oprire subscriber...$(COLOR_RESET)"
	@pkill -f "mqtt_subscriber" 2>/dev/null || true
	@echo ""
	@echo "$(COLOR_GREEN)Demo complet!$(COLOR_RESET)"

# Rulează doar simulatorul de senzori
run-sensor: posix
	@echo "$(COLOR_CYAN)Pornire simulator senzori (Ctrl+C pentru oprire)...$(COLOR_RESET)"
	$(POSIX_DIR)/virtual_sensor -v

# Rulează publisher interactiv
run-pub: posix
	@echo "$(COLOR_CYAN)Publisher interactiv (format: topic:message)$(COLOR_RESET)"
	$(POSIX_DIR)/mqtt_publisher

# Rulează subscriber pe toate topic-urile
run-sub: posix
	@echo "$(COLOR_CYAN)Subscriber pe toate topic-urile...$(COLOR_RESET)"
	$(POSIX_DIR)/mqtt_subscriber -t '#'

# Demo cu broker Mosquitto (necesită docker-compose up)
run-demo-paho: paho
	@echo ""
	@echo "$(COLOR_BOLD)═══════════════════════════════════════════════════════════$(COLOR_RESET)"
	@echo "$(COLOR_BOLD)        MQTT PAHO DEMO - Broker Mosquitto                  $(COLOR_RESET)"
	@echo "$(COLOR_BOLD)═══════════════════════════════════════════════════════════$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_YELLOW)Notă: Asigură-te că Mosquitto rulează (docker-compose up -d)$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_CYAN)Pornire subscriber...$(COLOR_RESET)"
	@$(PAHO_DIR)/mqtt_subscriber -t sensors/# &
	@sleep 2
	@echo ""
	@echo "$(COLOR_CYAN)Pornire sensor simulator (10 mesaje)...$(COLOR_RESET)"
	@$(PAHO_DIR)/virtual_sensor -n 10 -v -i 500
	@echo ""
	@pkill -f "mqtt_subscriber" 2>/dev/null || true
	@echo "$(COLOR_GREEN)Demo Paho complet!$(COLOR_RESET)"

# ==============================================================================
# TARGETS DE DEBUG ȘI TESTARE
# ==============================================================================

# Compilare cu debug și sanitizers
debug: CFLAGS += -O0 -g3 -fsanitize=address -fsanitize=undefined
debug: LIBS_POSIX += -fsanitize=address -fsanitize=undefined
debug: clean posix
	@echo "$(COLOR_YELLOW)Compilat cu AddressSanitizer și UBSan$(COLOR_RESET)"

# Verificare memorie cu Valgrind
valgrind: posix
	@echo "$(COLOR_CYAN)Rulare Valgrind pe publisher...$(COLOR_RESET)"
	valgrind --leak-check=full --show-leak-kinds=all \
		$(POSIX_DIR)/mqtt_publisher test/topic "test message"

# Test automat simplu
test: posix
	@echo ""
	@echo "$(COLOR_BOLD)Test automat POSIX...$(COLOR_RESET)"
	@echo ""
	@# Pornește subscriber care salvează în fișier
	@$(POSIX_DIR)/mqtt_subscriber -t test/# -n 3 -j > /tmp/mqtt_test_out.json &
	@sleep 1
	@# Trimite 3 mesaje
	@$(POSIX_DIR)/mqtt_publisher test/a "message1"
	@$(POSIX_DIR)/mqtt_publisher test/b "message2"
	@$(POSIX_DIR)/mqtt_publisher test/c "message3"
	@sleep 1
	@# Verifică output
	@if grep -q "message1" /tmp/mqtt_test_out.json && \
	    grep -q "message2" /tmp/mqtt_test_out.json && \
	    grep -q "message3" /tmp/mqtt_test_out.json; then \
		echo "$(COLOR_GREEN)✓ Test PASSED - Toate mesajele primite$(COLOR_RESET)"; \
	else \
		echo "$(COLOR_RED)✗ Test FAILED$(COLOR_RESET)"; \
		cat /tmp/mqtt_test_out.json; \
		exit 1; \
	fi
	@rm -f /tmp/mqtt_test_out.json

# ==============================================================================
# CLEANUP
# ==============================================================================

clean:
	@echo "$(COLOR_CYAN)Curățare...$(COLOR_RESET)"
	rm -rf $(BUILD_DIR)
	rm -f /dev/shm/mqtt_sim_*
	rm -f /dev/shm/sem.mqtt_sim_*
	@echo "$(COLOR_GREEN)✓ Curățat$(COLOR_RESET)"

# Curăță și shared memory (pentru POSIX)
clean-shm:
	@echo "$(COLOR_CYAN)Curățare shared memory...$(COLOR_RESET)"
	rm -f /dev/shm/mqtt_sim_*
	rm -f /dev/shm/sem.mqtt_sim_*
	@echo "$(COLOR_GREEN)✓ Shared memory curățată$(COLOR_RESET)"

# ==============================================================================
# HELP
# ==============================================================================

help:
	@echo ""
	@echo "$(COLOR_BOLD)Simulatoare MQTT - Makefile$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_CYAN)Compilare:$(COLOR_RESET)"
	@echo "  make posix       Compilează cu backend POSIX (fără dependențe)"
	@echo "  make paho        Compilează cu Paho MQTT (necesită libpaho)"
	@echo "  make all         Compilează ambele variante"
	@echo "  make debug       Compilează cu sanitizers pentru debugging"
	@echo ""
	@echo "$(COLOR_CYAN)Rulare:$(COLOR_RESET)"
	@echo "  make run-demo    Demo complet pub/sub POSIX"
	@echo "  make run-sensor  Rulează simulator senzori"
	@echo "  make run-pub     Publisher interactiv"
	@echo "  make run-sub     Subscriber pe toate topic-urile"
	@echo "  make run-demo-paho  Demo cu broker Mosquitto"
	@echo ""
	@echo "$(COLOR_CYAN)Testare:$(COLOR_RESET)"
	@echo "  make test        Test automat simplu"
	@echo "  make valgrind    Verificare memorie"
	@echo ""
	@echo "$(COLOR_CYAN)Curățare:$(COLOR_RESET)"
	@echo "  make clean       Șterge binare și build/"
	@echo "  make clean-shm   Curăță shared memory POSIX"
	@echo ""
	@echo "$(COLOR_CYAN)Binare generate:$(COLOR_RESET)"
	@echo "  build/posix/     - Versiuni fără dependențe externe"
	@echo "  build/paho/      - Versiuni cu Paho MQTT C"
	@echo ""
