# ==============================================================================
# tarsau - SAU Arsiv Araci Makefile
# ==============================================================================

CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -pedantic -std=c11
CFLAGS     += -Wshadow -Wconversion -Wstrict-prototypes
CFLAGS     += -Wmissing-prototypes -Wold-style-definition
CFLAGS     += -g
INCLUDES    = -Iinclude

SRC_DIR     = src
OBJ_DIR     = obj
BIN_DIR     = bin

TARGET      = $(BIN_DIR)/tarsau
SRCS        = $(wildcard $(SRC_DIR)/*.c)
OBJS        = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all dirs clean rebuild run

all: dirs $(TARGET)
	@echo "===== tarsau derlendi: ./$(TARGET) ====="

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)
	@echo "Temizlendi."

rebuild: clean all

run: all
	./$(TARGET) -h
