CC=gcc
CFLAGS=-Iinclude -Wall -O2
SRC=src/canvas.c src/math3d.c src/lighting.c src/renderer.c
DEMO=demo/main.c
TEST=demo/simple_test.c
OBJ=$(SRC:.c=.o)
TARGET=build/demo
TEST_TARGET=build/test

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(SRC) $(DEMO)
	$(CC) $(CFLAGS) $(SRC) $(DEMO) -o $(TARGET) -lm

$(TEST_TARGET): $(SRC) $(TEST)
	$(CC) $(CFLAGS) $(SRC) $(TEST) -o $(TEST_TARGET) -lm

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJ)

run: all
	./$(TARGET)

test: all
	./$(TEST_TARGET)