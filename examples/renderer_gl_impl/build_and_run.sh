gcc \
main.c \
../../utils/renderer_gl_impl.c \
../../utils/opengl.c \
../../utils/window.c \
../../utils/allocator.c \
../../utils/tools/memory_tracker.c \
-lSDL3 \
-lGL \
-lGLEW \
-o gl_test \
&& ./gl_test
