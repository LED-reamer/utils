#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//macros
#define IV2(x, y) (iv2){x, y}
#define IV3(x, y, z) (iv3){x, y, z}
#define IV4(x, y, z, w) (iv4){x, y, z, w}

#define UV2(x, y) (uv2){x, y}
#define UV3(x, y, z) (uv3){x, y, z}
#define UV4(x, y, z, w) (uv4){x, y, z, w}

#define V2(x, y) (v2){x, y}
#define V3(x, y, z) (v3){x, y, z}
#define V4(x, y, z, w) (v4){x, y, z, w}

#define V2_COPY(target, source) do { target[0] = source[0]; target[1] = source[1]; } while(0)
#define V3_COPY(target, source) do { target[0] = source[0]; target[1] = source[1]; target[2] = source[2];} while(0)
#define V4_COPY(target, source) do { target[0] = source[0]; target[1] = source[1]; target[2] = source[2]; target[3] = source[3];} while(0)

#define V2_SET(target, x, y) do { target[0] = x; target[1] = y; } while(0)
#define V3_SET(target, x, y, z) do { target[0] = x; target[1] = y; target[2] = z;} while(0)
#define V4_SET(target, x, y, z, w) do { target[0] = x; target[1] = y; target[2] = z; target[3] = w;} while(0)

//integer
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t umax;

//real
typedef float f32;
typedef double f64;

//vector
typedef i32 iv2[2];
typedef i32 iv3[3];
typedef i32 iv4[4];

typedef u32 uv2[2];
typedef u32 uv3[3];
typedef u32 uv4[4];

typedef f32 v2[2];
typedef f32 v3[3];
typedef f32 v4[4];
