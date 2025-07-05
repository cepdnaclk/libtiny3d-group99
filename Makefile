CC = gcc
CFLAGS = -Wall -Wextra -O3 -Iinclude
LDFLAGS = -lm

SRC_DIR = src
BUILD_DIR = build
DEMO_DIR = demo

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

LIB_NAME = libtiny3d.a
DEMO_NAME = demo

all: $(BUILD_DIR)/$(LIB_NAME) $(DEMO_DIR)/$(DEMO_NAME)

$(BUILD_DIR)/$(LIB_NAME): $(OBJS)
	ar rcs $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEMO_DIR)/$(DEMO_NAME): $(DEMO_DIR)/main.c $(BUILD_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $< -o $@ -L$(BUILD_DIR) -ltiny3d $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(DEMO_DIR)/$(DEMO_NAME)

.PHONY: all clean