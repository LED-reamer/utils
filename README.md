# utils
My personal utility library for my c projects.

NOTE: this is a fresh repo and I'm currently working on the examples and minor bugs / design choices
\nNOTE: gpu.h and renderer.h are still under heavy development

## content
|file				|has example		|linkerflags|description|
|-------------------|:-----------------:|-|-----------|
|allocator.h		|yes				||wrapper for allocation functions to keep track of what other functions allocate|
|arena.h			|yes				||simple memory arena implementation. Resizes automatically in blocks when allocating, free once|
|arguments.h		|yes				||parser for argv argc|
|array.h			|yes				||simple array that resizes in blocks|
|buffer.h			|yes				||a buffer to contain memory, read from files, write to files, also for parsing structs|
|dynamic_library.h	|yes				|-ldl|works on windows/linux|
|error.h			|yes				|||
|gpu.h				|in development		|-lSDL3||
|hashmap.h			|no					|||
|image.h			|yes				|-lm||
|linked_list.h		|no					||			|
|logging.h			|yes				||			|
|macros.h			|no					||			|
|random.h			|no					||			|
|renderer.h			|in development		||			|
|string.h			|no					||			|
|terminal.h			|no					||			|
|thread.h			|no					||			|
|types.h			|no					||			|
|window.h			|no					|-lSDL3|			|
|format/stl.h		|no					||			|
|format/wav.h		|no					||			|
|tools/assets.h		|no					||			|
|tools/debugger.h	|no					||			|
