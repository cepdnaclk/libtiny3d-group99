CC = gcc
CFLAGS = -Iinclude -Wall -O2
LDFLAGS = -lm
SRC_DIR = src
BUILD_DIR = build

# Core objects
CORE_OBJS = canvas.o math3d.o renderer.o lighting.o

# Targets
all: $(BUILD_DIR)/libtiny3d.a demo

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/libtiny3d.a: $(addprefix $(BUILD_DIR)/, $(CORE_OBJS))
	ar rcs $@ $^

demo: main.c $(BUILD_DIR)/libtiny3d.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) demo

.PHONY: all clean