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
	float xy[2];
	struct{float x, y;};
}vec2f;

typedef union {
	float xyz[3];
	struct{vec2f xy; float __pad1;};
	struct{float __pad2; vec2f yz;};
	struct{float x, y, z;};
}vec3f;

typedef union {
	float xyzw[4];
	struct{vec2f xy; float __pad1; float __pad2;};
	struct{float __pad3; vec2f yz; float __pad4;};
	struct{vec3f xyz; float __pad5;};
	struct{float __pad6; vec3f yzw;};
	struct{float x, y, z, w;};
}vec4f;

//float complex
typedef union {
	float ri[2];
	struct{float re, im;};
}complexf;

//float color rgba
typedef union {
	float rgba[4];
	struct{vec2f rg; float __pad1; float __pad2;};
	struct{float __pad3; vec2f gb; float __pad4;};
	struct{float __pad5; float __pad6; vec2f ba;};
	struct{vec3f rgb; float __pad7;};
	struct{float __pad8; vec3f gba;};
	struct{float r, g, b, a;};
}colorf;

//float rectangle
typedef union {
	float xywh[4];
	struct{vec2f pos; float __pad1; float __pad2;};
	struct{float __pad3; float __pad4; vec2f size;};
	struct{float x, y, w, h;};
}rectanglef;

// 4x4 matrix
typedef union {
	float m16[16];
	struct{ float m[4][4]; };		//m[col][row]
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
#endif
