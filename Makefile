CC = gcc
CFLAGS = -Iinclude -Wall -O2

LIB_NAME = build/libtiny3d.a
OBJ_FILES = build/canvas.o build/math3d.o build/renderer.o build/lighting.o

# Source to object rules
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the static library
$(LIB_NAME): $(OBJ_FILES)
	ar rcs $(LIB_NAME) $(OBJ_FILES)

# Main demo build
build/demo: $(LIB_NAME) demo/main.c
	$(CC) $(CFLAGS) demo/main.c -Lbuild -ltiny3d -o build/demo

# Pipeline test
build/test_pipeline: $(LIB_NAME) tests/test_pipeline.c
	$(CC) $(CFLAGS) tests/test_pipeline.c -Lbuild -ltiny3d -o build/test_pipeline

# Math test
build/test_math: $(LIB_NAME) tests/test_math.c
	$(CC) $(CFLAGS) tests/test_math.c -Lbuild -ltiny3d -o build/test_math

# Visual test: test_lines
build/test_lines: $(LIB_NAME) tests/visual_tests/test_lines.c
	$(CC) $(CFLAGS) tests/visual_tests/test_lines.c -Lbuild -ltiny3d -o build/test_lines

# Visual test: test_clip
build/test_clip: $(LIB_NAME) tests/visual_tests/test_clip.c
	$(CC) $(CFLAGS) tests/visual_tests/test_clip.c -Lbuild -ltiny3d -o build/test_clip

# Visual test: test_soccerball
build/test_soccerball: $(LIB_NAME) tests/visual_tests/test_soccerball.c
	$(CC) $(CFLAGS) tests/visual_tests/test_soccerball.c -Lbuild -ltiny3d -o build/test_soccerball

# Visual test: test_lighting
build/test_lighting: $(LIB_NAME) tests/visual_tests/test_lighting.c
	$(CC) $(CFLAGS) tests/visual_tests/test_lighting.c -Lbuild -ltiny3d -o build/test_lighting

# Build everything
all: $(LIB_NAME) build/demo build/test_pipeline build/test_math build/test_lines build/test_clip build/test_soccerball build/test_lighting

# Clean build artifacts
clean:
	rm -rf build/* demo/*.pgm visual_tests/*.pgm
