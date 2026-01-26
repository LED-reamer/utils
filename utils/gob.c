#include "deserialize.h"

uint8_t varint_read_u64(const void* data, size_t size, uint64_t* out_value){
	const uint8_t *bytes = data;
    if (size > 0 && bytes[0] < 128) {
        *out_value = bytes[0];
        return 1;
    }
    uint64_t b;
    *out_value = 0;
    uint8_t i = 0;
    while (i < size && i < 10) {
        b = bytes[i]; 
        *out_value |= (b & 127) << (7 * i); 
        if (b < 128) {
            return i + 1;
        }
        i++;
    }
    //return i == 10 ? -1 : 0;
    return 0;
}

uint8_t varint_read_i64(const void* data, size_t size, int64_t* out_value){
	uint64_t ux;
    uint8_t n = varint_read_u64(data, size, &ux);
    *out_value = (int64_t)(ux >> 1);
    *out_value = ux&1 ? ~*out_value : *out_value;
    return n;
}

int8_t varint_write_u64(void* data, size_t size, uint64_t value){
	if(size < 1) return -1;
	uint8_t *bytes = data;
    if (value < 128) {
        *bytes = value;
        return 1;
    }
    uint8_t n = 0;
    do {
    	if(n >= size) return -1;
        bytes[n++] = (uint8_t)value | 128;
        value >>= 7;
    } while (value >= 128);
    if(n >= size) return -1;
    bytes[n++] = (uint8_t)value;
    return n;
}

int8_t varint_write_i64(void* data, size_t size, int64_t value){
	uint64_t ux = (uint64_t)value << 1;
	ux = value < 0 ? ~ux : ux;
	return varint_write_u64(data, size, ux);
}
