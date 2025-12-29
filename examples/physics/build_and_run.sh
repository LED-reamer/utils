gcc \
main.c \
../../utils/physics.c \
../../utils/renderer.c \
../../utils/opengl.c \
../../utils/window.c \
../../utils/arena.c \
../../utils/font.c \
../../utils/buffer.c \
../../utils/allocator.c \
../../utils/hashmap.c \
../../utils/tools/memory_tracker.c \
-lSDL3 \
-lGL \
-lGLEW \
-lm \
-o physics_test \
&& ./physics_test
