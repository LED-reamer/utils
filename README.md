# utils
My personal utility library for my c projects.

NOTE: this is a fresh repo and I'm currently working on the examples and minor bugs / design choices
\nNOTE: gpu.h and renderer.h are still under heavy development

## content
|file				|has example		|linkerflags|description|
|-------------------|:-----------------:|-|-----------|
|[allocator.h](/utils/allocator.h)|yes||wrapper for allocation functions to keep track of what other functions allocate|
|[arena.h](/utils/arena.h)|yes||simple memory arena implementation. Resizes automatically in blocks when allocating, free once|
|[arguments.h](/utils/arguments.h)|yes||parser for argv argc|
|[array.h](/utils/array.h)|yes||simple array that resizes in blocks|
|[buffer.h](/utils/buffer.h)|yes||a buffer to contain memory, read from files, write to files, also for parsing structs|
|[dynamic_library.h](/utils/dynamic_library.h)|yes|-ldl|works on windows/linux|
|[error.h](/utils/error.h)|yes|||
|[gpu.h](/utils/gpu.h)|in development|-lSDL3||
|[hashmap.h](/utils/hashmap.h)|no|||
|[image.h](/utils/image.h)|yes|-lm||
|[linked_list.h](/utils/linked_list.h)|no||			|
|[logging.h](/utils/logging.h)|yes||			|
|[macros.h](/utils/macros.h)|no||			|
|[random.h](/utils/random.h)|no||			|
|[renderer.h](/utils/renderer.h)|in development||			|
|[string.h](/utils/string.h)|no||			|
|[terminal.h](/utils/terminal.h)|no||			|
|[thread.h](/utils/thread.h)|no||			|
|[types.h](/utils/types.h)|no||			|
|[window.h](/utils/window.h)|no|-lSDL3|			|
|[format/stl.h](/utils/format/stl.h)|no||			|
|[format/wav.h](/utils/format/wav.h)|no||			|
|[tools/assets.h](/utils/tools/assets.h)|no||			|
|[tools/debugger.h](/utils/tools/debugger.h)|no||			|
