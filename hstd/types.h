#ifndef TYPES_H
#define TYPES_H

typedef unsigned char b8;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long s64;

typedef float f32;
typedef double f64;
typedef long double f128;

// vectors

//signed
typedef union {
	s32 xy[2];
	struct{s32 x, y;};
}vec2s;

typedef union {
	s32 xyz[3];
	struct{vec2s xy; s32 __pad1;};
	struct{s32 __pad2; vec2s yz;};
	struct{s32 x, y, z;};
}vec3s;

typedef union {
	s32 xyzw[4];
	struct{vec2s xy; s32 __pad1; s32 __pad2;};
	struct{s32 __pad3; vec2s yz; s32 __pad4;};
	struct{vec3s xyz; s32 __pad5;};
	struct{s32 __pad6; vec3s yzw;};
	struct{s32 x, y, z, w;};
}vec4s;

//unsigned
typedef union {
	u32 xy[2];
	struct{u32 x, y;};
}vec2u;

typedef union {
	u32 xyz[3];
	struct{vec2u xy; u32 __pad1;};
	struct{u32 __pad2; vec2u yz;};
	struct{u32 x, y, z;};
}vec3u;

typedef union {
	u32 xyzw[4];
	struct{vec2u xy; u32 __pad1; u32 __pad2;};
	struct{u32 __pad3; vec2u yz; u32 __pad4;};
	struct{vec3u xyz; u32 __pad5;};
	struct{u32 __pad6; vec3u yzw;};
	struct{u32 x, y, z, w;};
}vec4u;

//float
typedef union {
	f32 xy[2];
	struct{f32 x, y;};
}vec2f;

typedef union {
	f32 xyz[3];
	struct{vec2f xy; f32 __pad1;};
	struct{f32 __pad2; vec2f yz;};
	struct{f32 x, y, z;};
}vec3f;

typedef union {
	f32 xyzw[4];
	struct{vec2f xy; f32 __pad1; f32 __pad2;};
	struct{f32 __pad3; vec2f yz; f32 __pad4;};
	struct{vec3f xyz; f32 __pad5;};
	struct{f32 __pad6; vec3f yzw;};
	struct{f32 x, y, z, w;};
}vec4f;

//float complex
typedef union {
	f32 ri[2];
	struct{f32 re, im;};
}complexf;

//float color rgba
typedef union {
	f32 rgba[4];
	struct{vec2f rg; f32 __pad1; f32 __pad2;};
	struct{f32 __pad3; vec2f gb; f32 __pad4;};
	struct{f32 __pad5; f32 __pad6; vec2f ba;};
	struct{vec3f rgb; f32 __pad7;};
	struct{f32 __pad8; vec3f gba;};
	struct{f32 r, g, b, a;};
}colorf;

//float rectangle
typedef union {
	f32 xywh[4];
	struct{vec2f pos; f32 __pad1; f32 __pad2;};
	struct{f32 __pad3; f32 __pad4; vec2f size;};
	struct{f32 x, y, w, h;};
}rectanglef;

// 4x4 matrix
typedef union {
	f32 m16[16];
	struct{ f32 m[4][4]; };		//m[col][row]
	struct{ vec4f c1, c2, c3, c4; };//collumns
}mat4x4;

//initializers
#define VEC2S(x, y) (vec2s){{x, y}}
#define VEC3S(x, y, z) (vec3s){{x, y, z}}
#define VEC4S(x, y, z, w) (vec4s){{x, y, z, w}}
#define VEC2U(x, y) (vec2u){{x, y}}
#define VEC3U(x, y, z) (vec3u){{x, y, z}}
#define VEC4U(x, y, z, w) (vec4u){{x, y, z, w}}
#define VEC2F(x, y) (vec2f){{x, y}}
#define VEC3F(x, y, z) (vec3f){{x, y, z}}
#define VEC4F(x, y, z, w) (vec4f){{x, y, z, w}}
#define COMPLEXF(re, im) (complexf){{re, im}}
#define COLORF(r, g, b, a) (colorf){{r, g, b, a}}
#define RECTANGLEF(x, y, w, h) (rectanglef){{x, y, w, h}}
#define MAT4X4(x) (mat4x4){{x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x}}

//c11 feature:
static inline vec2s __vec2s(s32 x, s32 y) { return (vec2s){{x, y}}; }
static inline vec2u __vec2u(u32 x, u32 y) { return (vec2u){{x, y}}; }
static inline vec2f __vec2f(f32 x, f32 y) { return (vec2f){{x, y}}; }
static inline vec3s __vec3s(s32 x, s32 y, s32 z) { return (vec3s){{x, y, z}}; }
static inline vec3u __vec3u(u32 x, u32 y, u32 z) { return (vec3u){{x, y, z}}; }
static inline vec3f __vec3f(f32 x, f32 y, f32 z) { return (vec3f){{x, y, z}}; }
static inline vec4s __vec4s(s32 x, s32 y, s32 z, s32 w) { return (vec4s){{x, y, z, w}}; }
static inline vec4u __vec4u(u32 x, u32 y, u32 z, u32 w) { return (vec4u){{x, y, z, w}}; }
static inline vec4f __vec4f(f32 x, f32 y, f32 z, f32 w) { return (vec4f){{x, y, z, w}}; }

#define vec2(X, Y) _Generic(((X)+(Y)), \
    s32: __vec2s, \
    u32: __vec2u, \
    f32: __vec2f \
)(X, Y)
#define vec3(X, Y, Z) _Generic(((X)+(Y)+(Z)), \
    s32: __vec3s, \
    u32: __vec3u, \
    f32: __vec3f \
)(X, Y, Z)
#define vec4(X, Y, Z, W) _Generic(((X)+(Y)+(Z)+(W)), \
    s32: __vec4s, \
    u32: __vec4u, \
    f32: __vec4f \
)(X, Y, Z, W)

#endif
