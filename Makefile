CC=gcc
CFLAGS=-Iinclude -Wall -O2
SRC=src/canvas.c src/math3d.c src/lighting.c src/renderer.c
DEMO=demo/main.c
OBJ=$(SRC:.c=.o)
TARGET=build/demo
#TEST=tests/test_math.c
#TARGET=build/test_math
all: $(TARGET)

$(TARGET): $(SRC) $(DEMO)
	$(CC) $(CFLAGS) $(SRC) $(DEMO) -o $(TARGET) -lm

clean:
	rm -f $(TARGET) $(OBJ)

run: all
	./$(TARGET)
#$(TARGET): $(SRC) $(TEST)
#	$(CC) $(CFLAGS) $(SRC) $(TEST) -o $(TARGET) -lm