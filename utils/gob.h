#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//https://www.cs.ubc.ca/~bestchai/teaching/cs416_2015w2/go1.4.3-docs/pkg/encoding/gob/index.html

//old links
//https://kreya.app/blog/protocolbuffers-wire-format/
//https://protobuf.dev/programming-guides/encoding/

//return bytes read/written
//buffer needs max 10 bytes for max varint
//when write failed returns -1


uint8_t varint_read_u64(const void* data, size_t size, uint64_t* out_value);
uint8_t varint_read_i64(const void* data, size_t size, int64_t* out_value);

int8_t varint_write_u64(void* data, size_t size, uint64_t value);
int8_t varint_write_i64(void* data, size_t size, int64_t value);
