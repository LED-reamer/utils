# utils
My personal utility library for my c projects.

NOTE: this is a fresh repo and I'm currently working on the examples and minor bugs / design choices
ALSO: gpu.h and renderer.h are still under heavy development

TODO: fix linked_list.h to use stack allocated head nodes AND use arena allocator

## content
|file				|has example		|linkerflags|description|
|-------------------|:-----------------:|-|-----------|
|[allocator.h](/utils/allocator.h)|yes||wrapper for allocation functions to keep track of what other functions allocate|
|[arena.h](/utils/arena.h)|yes||simple memory arena implementation. Resizes automatically in blocks when allocating, free once|
|[arguments.h](/utils/arguments.h)|yes||parser for argv argc|
|[array.h](/utils/array.h)|yes||simple array that resizes in blocks|
|[buffer.h](/utils/buffer.h)|yes||a buffer to contain memory, read from files, write to files, also for parsing structs|
|[dynamic_library.h](/utils/dynamic_library.h)|yes|-ldl|loads .so/.dll libraries. works on windows/linux|
|[error.h](/utils/error.h)|yes||simple error struct that contains state and custom error strings. Intended to be returned by all functions|
|[gpu.h](/utils/gpu.h)|in development|-lSDL3|wrapper around [SDL3's sdl_gpu.h](https://wiki.libsdl.org/SDL3/CategoryGPU)|
|[hashmap.h](/utils/hashmap.h)|yes||a hashmap|
|[image.h](/utils/image.h)|yes|-lm|basically just a wrapper around stb_image and stb_image_write|
|[linked_list.h](/utils/linked_list.h)|yes||a linked list|
|[logging.h](/utils/logging.h)|yes||simple logging and error tracking|
|[macros.h](/utils/macros.h)|no||(hopefully)useful macros|
|[random.h](/utils/random.h)|yes||generate random values, GUIDs and 2D perlin noise|
|[renderer.h](/utils/renderer.h)|in development|-lSDL3|uses gpu.h to quickly draw shapes (both 2D and 3D) and textures|
|[string.h](/utils/string.h)|yes||stack allocated strings|
|[terminal.h](/utils/terminal.h)|no|-lncurses|api for reading input and drawing text/buffers/images to the terminal|
|[thread.h](/utils/thread.h)|yes|-lpthread or windows|threading. For both linux and windows|
|[types.h](/utils/types.h)|no||vector and matrix operations.|
|[window.h](/utils/window.h)|yes|-lSDL3|Everything about windows and input (uses SDL3)|
|[format/stl.h](/utils/format/stl.h)|no||read 3d mesh files in the .stl format|
|[format/wav.h](/utils/format/wav.h)|no||read sound files in the .wav format|
|[tools/assets.h](/utils/tools/assets.h)|no||load files from zip files/from disc|
|[tools/memory_tracker.h](/utils/tools/memory_tracker.h)|no||tracks memory allocations|
