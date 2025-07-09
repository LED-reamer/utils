# Coding Style Conventions

Please follow the conventions of the project.

Here is a small collection of my code conventions.

|code element|convention|example|
|-|-|-|
|defines|ALL_CAPS|`#define MY_DEFINITION`|
|macros|ALL_CAPS|`#define MIN(a,b) (((a)<(b))?(a):(b))`|
|all variables|lower_case|`int my_variable = 50;`|
|functions|lowercase|`window_create()`|
|types/structs|lowercase + _t|`typedef struct{int my_fields;} my_struct_t;`|
|enums|lowercase + _e|`typedef enum{WINDOW_MAXIMIZED, WINDOW_MINIMIZED} window_state_e;`|
|enum members|ENUMS_IN_CAPS starting with enum type|`WINDOW_MAXIMIZED`|
|integers|use stdint.h and ALWAYS use uint when possible|`uint32_t, uint8_t, int16_t`|

- Curly brackets always open like this (I don't like it but it's much more compact)

```
if(condition){
	
}else{
	
}

void func(){
	
}
```

**NEVER allocate using malloc (/realloc/calloc) directly**

use allocators from utils/allocator.h instead

`object_t object_create(allocator_t* allocator, int data);`

the object then needs to keep track of it's allocator
so it can use it again to free when calling

`void object_destroy(object_t* obj);`


All objects should be handeled this way!

Note: try to allocate the object_t on the stack to prevent useless allocations
