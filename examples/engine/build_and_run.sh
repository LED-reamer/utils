gcc \
main.c \
utils/tools/engine.c \
utils/tools/ecs.c \
utils/allocator.c \
utils/image.c \
utils/window.c \
utils/buffer.c \
utils/random.c \
utils/hashmap.c \
utils/linked_list.c \
utils/tools/aura.c \
utils/tools/debugger.c \
utils/tools/memory_tracker.c \
utils/tools/assets.c \
utils/arena.c \
-lSDL3 -lm -lzip \
-Wall \
-o game && ./game
