gcc \
main.c \
../../utils/renderer.c \
../../utils/opengl.c \
../../utils/window.c \
../../utils/arena.c \
../../utils/font.c \
../../utils/buffer.c \
../../utils/allocator.c \
../../utils/tools/memory_tracker.c \
-lSDL3 \
-lGL \
-lGLEW \
-lm \
-o gl_test \
&& ./gl_test
