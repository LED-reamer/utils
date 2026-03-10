#ifndef ARRAY_H
#define ARRAY_H
#include <stddef.h>
#include <stdint.h>

#ifndef ARRAY_REALLOC
#define ARRAY_REALLOC realloc
#endif
#ifndef ARRAY_FREE
#define ARRAY_FREE free
#endif

#define MAKE_ARRAY(name, type) \
typedef struct { type* items; size_t count; size_t capacity;} name;

#define array_push(array_ptr, item) \
do { array_resize((array_ptr), (array_ptr)->count+1); (array_ptr)->items[(array_ptr)->count++] = item; } while(0)

#define array_pop(array_ptr) \
({ typeof(*((array_ptr)->items)) ret = (typeof(*((array_ptr)->items))){0}; if((array_ptr)->count != 0){ ret = (array_ptr)->items[(array_ptr)->count-1]; array_resize((array_ptr), (array_ptr)->count-1); (array_ptr)->count--; } ret;})

#define array_free(array_ptr) \
do { if((array_ptr)->capacity != 0) ARRAY_FREE((array_ptr)->items); (array_ptr)->items = NULL; (array_ptr)->count = 0; (array_ptr)->capacity = 0; } while(0)

static inline size_t next_capacity(uint64_t count) {
    if (count == 0) return 0;
    count--;
    count |= count >> 1; count |= count >> 2; count |= count >> 4; count |= count >> 8; count |= count >> 16; count |= count >> 32;
    return count + 1;
}

#define array_resize(array_ptr, new_count) \
do { if(next_capacity(new_count) != (array_ptr)->capacity) { (array_ptr)->capacity = next_capacity(new_count); (array_ptr)->items = ARRAY_REALLOC((array_ptr)->items, (array_ptr)->capacity * sizeof(*((array_ptr)->items))); } } while(0)

#define array_fit_size(array_ptr) \
do { if((array_ptr)->count != (array_ptr)->capacity) { (array_ptr)->capacity = (array_ptr)->count; (array_ptr)->items = ARRAY_REALLOC((array_ptr)->items, (array_ptr)->capacity * sizeof(*((array_ptr)->items))); } } while(0)

#define array_len(array_ptr) \
({(array_ptr)->count;})

#define array_item_size(array_ptr) \
({sizeof(*((array_ptr)->items));})

#define array_size(array_ptr) \
({(array_ptr)->count * array_item_size((array_ptr));})

//int (*func_negative_if_a_less_b_etc)(const void* a, const void* b));
#define array_sort(array_ptr, func_negative_if_a_less_b_etc) \
do { qsort((array_ptr)->items, (array_ptr)->count, array_item_size((array_ptr)), func_negative_if_a_less_b_etc); } while(0)
#endif
