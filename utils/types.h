#pragma once
#include <float.h>	//FLT_EPSILON
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>	 //for memcpy

// === VECTOR2 (vec2_t) ===
// vec2(x, y)                 	- Constructor
// vec2_add(a, b)             	- Addition
// vec2_sub(a, b)             	- Subtraction
// vec2_scale(v, s)           	- Scalar multiplication
// vec2_negate(v)             	- Negation
// vec2_lerp(a, b, t)         	- Linear interpolation
// vec2_dot(a, b)             	- Dot product
// vec2_length(v)             	- Length
// vec2_length_squared(v)     	- Length squared (for comparisons)
// vec2_normalize(v)          	- Normalization
// vec2_rotate_around(p, o, r) 	- Rotate around point (r in radians)
// vec2_distance(a, b)        	- Distance between points

// === VECTOR3 (vec3_t) ===
// vec3(x, y, z)              				- Constructor
// vec3_add(a, b)             				- Addition
// vec3_sub(a, b)             				- Subtraction
// vec3_scale(v, s)           				- Scalar multiplication
// vec3_negate(v)             				- Negation
// vec3_lerp(a, b, t)         				- Linear interpolation
// vec3_cross(a, b)           				- Cross product
// vec3_dot(a, b)             				- Dot product
// vec3_length(v)             				- Length
// vec3_length_squared(v)     				- Length squared (for comparisons)
// vec3_normalize(v)          				- Normalization
// vec3_rotate_around_axis(p, o, axis, r) 	- Rotate around axis & point (r in radians)
// vec3_distance(a, b)        				- Distance between points

// === MATRIX4x4 (mat4x4_t) === (column major => matrix[col][row])
// mat4x4(x)                  							- Constructor (all values = x)
// mat4x4_copy_array(a)                  				- Constructor copies float a[16]
// mat4x4_identity()          							- Identity matrix
// mat4x4_mul(a, b)           							- Matrix multiplication (returns new matrix)
// mat4x4_mul_vec4(a, vec4)								- multiplies Matrix with vec4 (returns new vec4)
// mat4x4_translate(out, x, y, z) 						- Translation (multiplies out)
// mat4x4_scale(out, x, y, z)     						- Scale (multiplies out)
// mat4x4_rotate(out, angle_rad, axis) 					- Rotation around axis (multiplies out)
// mat4x4_lookat(eye, center, up) 						- Camera (view) matrix, eye -> camera position, center -> position to look at
// mat4x4_ortho(left, right, bottom, top, near, far) 	- Orthographic projection
// mat4x4_perspective(fovy_rad, aspect, near, far) 		- Perspective projection matrix
// mat4x4_transpose(m) 									- Returns the transpose of matrix m
// mat4x4_inverse(m) 									- Returns the inverse of matrix m (or identity if not invertible)

// === MISC ===
// deg2rad(degrees)           - Degrees to radians conversion
// rad2deg(rad)           - Radians to degrees conversion

//(forward-)declarations:
// signed
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
// unsigned
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
// unsigned max
typedef size_t umax;
// real
typedef float r32;
typedef double r64;
typedef long double r128;
// vectors
typedef union vec2_t vec2_t;
typedef union vec3_t vec3_t;
typedef union vec4_t vec4_t;
typedef union ivec2_t ivec2_t;
typedef union ivec3_t ivec3_t;
typedef union ivec4_t ivec4_t;
typedef union uvec2_t uvec2_t;
typedef union uvec3_t uvec3_t;
typedef union uvec4_t uvec4_t;
// matrices
typedef union mat4x4_t mat4x4_t;  // inline functions use OpenGL friendly m[column][row] system
// misc.
typedef union color_t color_t;
typedef union rectangle_t rectangle_t;

// macro
#define vec2(x, y) \
	(vec2_t) {     \
		{ x, y }   \
	}
#define vec3(x, y, z) \
	(vec3_t) {        \
		{ x, y, z }   \
	}
#define vec4(x, y, z, w) \
	(vec4_t) {           \
		{ x, y, z, w }   \
	}
#define ivec2(x, y) \
	(ivec2_t) {     \
		{ x, y }    \
	}
#define ivec3(x, y, z) \
	(ivec3_t) {        \
		{ x, y, z }    \
	}
#define ivec4(x, y, z, w) \
	(ivec4_t) {           \
		{ x, y, z, w }    \
	}
#define uvec2(x, y) \
	(uvec2_t) {     \
		{ x, y }    \
	}
#define uvec3(x, y, z) \
	(uvec3_t) {        \
		{ x, y, z }    \
	}
#define uvec4(x, y, z, w) \
	(uvec4_t) {           \
		{ x, y, z, w }    \
	}
#define color(r, g, b, a) \
	(color_t) {           \
		{ r, g, b, a }    \
	}
#define rectangle(x, y, w, h) \
	(rectangle_t) {           \
		{ x, y, w, h }        \
	}
#define mat4x4(x)                                                        \
	(mat4x4_t) {                                                         \
		{                                                                \
			{                                                            \
				{x, x, x, x}, {x, x, x, x}, {x, x, x, x}, { x, x, x, x } \
			}                                                            \
		}                                                                \
	}

#define PI 3.14159265358979323846f

//(forward declared) structs/unions:

typedef union vec2_t {
	struct {
		float x, y;
	};
	float xy[2];
} vec2_t;

typedef union vec3_t {
	struct {
		float x, y, z;
	};
	float xyz[3];
} vec3_t;

typedef union vec4_t {
	struct {
		float x, y, z, w;
	};
	float xyzw[4];
} vec4_t;

typedef union ivec2_t {
	struct {
		int32_t x, y;
	};
	int32_t xy[2];
} ivec2_t;

typedef union ivec3_t {
	struct {
		int32_t x, y, z;
	};
	int32_t xyz[3];
} ivec3_t;

typedef union ivec4_t {
	struct {
		int32_t x, y, z, w;
	};
	int32_t xyzw[4];
} ivec4_t;

typedef union uvec2_t {
	struct {
		uint32_t x, y;
	};
	uint32_t xy[2];
} uvec2_t;

typedef union uvec3_t {
	struct {
		uint32_t x, y, z;
	};
	uint32_t xyz[3];
} uvec3_t;

typedef union uvec4_t {
	struct {
		uint32_t x, y, z, w;
	};
	uint32_t xyzw[4];
} uvec4_t;

typedef union mat4x4_t {
	struct {
		float m[4][4];	// m[col][row]
	};
	float m16[16];
} mat4x4_t;

typedef union color_t {
	struct {
		float r, g, b, a;
	};
	float rgba[4];
} color_t;

typedef union rectangle_t {
	struct {
		float x, y, w, h;
	};
	float xywh[4];
} rectangle_t;

// inline functions
static inline float deg2rad(float degrees) {
	return degrees * (PI / 180.0f);
}

static inline float rad2deg(float rad) {
	return rad * (180.0f / PI);
}

static inline vec2_t vec2_add(vec2_t a, vec2_t b) {
	return vec2(a.x + b.x, a.y + b.y);
}

static inline vec2_t vec2_sub(vec2_t a, vec2_t b) {
	return vec2(a.x - b.x, a.y - b.y);
}

static inline vec2_t vec2_scale(vec2_t v, float s) {
	return vec2(v.x * s, v.y * s);
}

static inline vec2_t vec2_negate(vec2_t v) {
	return vec2(-v.x, -v.y);
}

static inline vec2_t vec2_lerp(vec2_t a, vec2_t b, float t) {
	return vec2(
		a.x + (b.x - a.x) * t,
		a.y + (b.y - a.y) * t);
}

static inline float vec2_dot(vec2_t a, vec2_t b) {
	return a.x * b.x + a.y * b.y;
}

// optimal for comparing lengths
static inline float vec2_length_squared(vec2_t v) {
	return vec2_dot(v, v);
}

static inline float vec2_length(vec2_t v) {
	return sqrtf(vec2_length_squared(v));
}

static inline float vec2_distance(vec2_t a, vec2_t b) {
	vec2_t diff = vec2_sub(a, b);
	return vec2_length(diff);
}

static inline vec2_t vec2_normalize(vec2_t v) {
	float len = vec2_length(v);
	if (len > FLT_EPSILON) {
		return vec2(v.x / len, v.y / len);
	}
	return v;
}

static inline vec2_t vec2_rotate_around(vec2_t point, vec2_t origin, float radians) {
	float s = sinf(radians);
	float c = cosf(radians);

	vec2_t p = vec2_sub(point, origin);

	float xnew = p.x * c - p.y * s;
	float ynew = p.x * s + p.y * c;

	return vec2_add(vec2(xnew, ynew), origin);
}

static inline vec3_t vec3_add(vec3_t a, vec3_t b) {
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline vec3_t vec3_sub(vec3_t a, vec3_t b) {
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline vec3_t vec3_scale(vec3_t v, float s) {
	return vec3(v.x * s, v.y * s, v.z * s);
}

static inline vec3_t vec3_negate(vec3_t v) {
	return vec3(-v.x, -v.y, -v.z);
}

static inline vec3_t vec3_lerp(vec3_t a, vec3_t b, float t) {
	return vec3(
		a.x + (b.x - a.x) * t,
		a.y + (b.y - a.y) * t,
		a.z + (b.z - a.z) * t);
}

static inline vec3_t vec3_cross(vec3_t a, vec3_t b) {
	return vec3(a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x);
}

static inline float vec3_dot(vec3_t a, vec3_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// optimal for comparing lengths
static inline float vec3_length_squared(vec3_t v) {
	return vec3_dot(v, v);
}

static inline float vec3_length(vec3_t v) {
	return sqrtf(vec3_length_squared(v));
}

static inline float vec3_distance(vec3_t a, vec3_t b) {
	vec3_t diff = vec3_sub(a, b);
	return vec3_length(diff);
}

static inline vec3_t vec3_normalize(vec3_t v) {
	float len = vec3_length(v);
	vec3_t normalized_vec = vec3(0, 0, 0);
	if (len > FLT_EPSILON) {
		normalized_vec.x = v.x / len;
		normalized_vec.y = v.y / len;
		normalized_vec.z = v.z / len;
	}
	return normalized_vec;
}

static inline vec3_t vec3_rotate_around_axis(vec3_t point, vec3_t origin, vec3_t axis, float radians) {
	axis = vec3_normalize(axis);

	vec3_t p = vec3_sub(point, origin);

	float cos_theta = cosf(radians);
	float sin_theta = sinf(radians);

	vec3_t term1 = vec3_scale(p, cos_theta);
	vec3_t term2 = vec3_scale(vec3_cross(axis, p), sin_theta);
	vec3_t term3 = vec3_scale(axis, vec3_dot(axis, p) * (1.0f - cos_theta));

	vec3_t rotated = vec3_add(vec3_add(term1, term2), term3);

	return vec3_add(rotated, origin);
}

static inline mat4x4_t mat4x4_copy_array(float array[16]) {
	mat4x4_t m;
	memcpy(m.m16, array, 16 * sizeof(float));
	return m;
}

static inline mat4x4_t mat4x4_identity() {
	mat4x4_t m = mat4x4(0);
	m.m16[0] = m.m16[5] = m.m16[10] = m.m16[15] = 1.0f;
	return m;
}

static inline mat4x4_t mat4x4_mul(const mat4x4_t* a, const mat4x4_t* b) {
	mat4x4_t result = mat4x4(0);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			float sum = 0.0f;
			for (int k = 0; k < 4; k++) {
				sum += a->m[k][j] * b->m[i][k];
			}
			result.m[i][j] = sum;
		}
	}
	return result;
}

static inline vec4_t mat4x4_mul_vec4(const mat4x4_t* m, const vec4_t* v) {
	return vec4(
		m->m[0][0]*v->x + m->m[1][0]*v->y + m->m[2][0]*v->z + m->m[3][0]*v->w,
		m->m[0][1]*v->x + m->m[1][1]*v->y + m->m[2][1]*v->z + m->m[3][1]*v->w,
		m->m[0][2]*v->x + m->m[1][2]*v->y + m->m[2][2]*v->z + m->m[3][2]*v->w,
		m->m[0][3]*v->x + m->m[1][3]*v->y + m->m[2][3]*v->z + m->m[3][3]*v->w
	);
}

static inline void mat4x4_translate(mat4x4_t* out, float x, float y, float z) {
	mat4x4_t t = mat4x4(0);
	t.m[0][0] = 1.0f;
	t.m[1][1] = 1.0f;
	t.m[2][2] = 1.0f;
	t.m[3][3] = 1.0f;
	t.m[3][0] = x;
	t.m[3][1] = y;
	t.m[3][2] = z;

	*out = mat4x4_mul(out, &t);
}

static inline void mat4x4_scale(mat4x4_t* out, float x, float y, float z) {
	mat4x4_t s = mat4x4(0);
	s.m[0][0] = x;
	s.m[1][1] = y;
	s.m[2][2] = z;
	s.m[3][3] = 1.0f;

	*out = mat4x4_mul(out, &s);
}

static inline void mat4x4_rotate(mat4x4_t* out, float angle_rad, vec3_t axis) {
	axis = vec3_normalize(axis);
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	float t = 1.0f - c;

	float x = axis.x, y = axis.y, z = axis.z;

	mat4x4_t r = mat4x4(0);
	r.m[0][0] = t * x * x + c;
	r.m[0][1] = t * x * y - s * z;
	r.m[0][2] = t * x * z + s * y;
	r.m[0][3] = 0.0f;

	r.m[1][0] = t * x * y + s * z;
	r.m[1][1] = t * y * y + c;
	r.m[1][2] = t * y * z - s * x;
	r.m[1][3] = 0.0f;

	r.m[2][0] = t * x * z - s * y;
	r.m[2][1] = t * y * z + s * x;
	r.m[2][2] = t * z * z + c;
	r.m[2][3] = 0.0f;

	r.m[3][0] = 0.0f;
	r.m[3][1] = 0.0f;
	r.m[3][2] = 0.0f;
	r.m[3][3] = 1.0f;

	*out = mat4x4_mul(out, &r);
}

static inline mat4x4_t mat4x4_lookat(vec3_t eye, vec3_t center, vec3_t up) {
	mat4x4_t m = {0};
	vec3_t f = vec3_normalize(vec3_sub(center, eye));
	vec3_t s = vec3_normalize(vec3_cross(f, up));
	vec3_t u = vec3_cross(s, f);

	m.m[0][0] = s.x;
	m.m[0][1] = u.x;
	m.m[0][2] = -f.x;
	m.m[1][0] = s.y;
	m.m[1][1] = u.y;
	m.m[1][2] = -f.y;
	m.m[2][0] = s.z;
	m.m[2][1] = u.z;
	m.m[2][2] = -f.z;
	m.m[3][0] = -vec3_dot(s, eye);
	m.m[3][1] = -vec3_dot(u, eye);
	m.m[3][2] = vec3_dot(f, eye);
	m.m[0][3] = m.m[1][3] = m.m[2][3] = 0.0f;
	m.m[3][3] = 1.0f;

	return m;
}
// left handed
static inline mat4x4_t mat4x4_lookat_lh(vec3_t eye, vec3_t center, vec3_t up) {
	mat4x4_t m = {0};
	vec3_t f = vec3_normalize(vec3_sub(center, eye));
	vec3_t s = vec3_normalize(vec3_cross(f, up));
	vec3_t u = vec3_cross(s, f);

	m.m[0][0] = s.x;
	m.m[0][1] = u.x;
	m.m[0][2] = f.x;
	m.m[1][0] = s.y;
	m.m[1][1] = u.y;
	m.m[1][2] = f.y;
	m.m[2][0] = s.z;
	m.m[2][1] = u.z;
	m.m[2][2] = f.z;

	m.m[3][0] = -vec3_dot(s, eye);
	m.m[3][1] = -vec3_dot(u, eye);
	m.m[3][2] = vec3_dot(f, eye);
	m.m[0][3] = m.m[1][3] = m.m[2][3] = 0.0f;
	m.m[3][3] = 1.0f;

	return m;
}

static inline mat4x4_t mat4x4_ortho(float left, float right, float bottom, float top, float near, float far) {
	mat4x4_t m = mat4x4_identity();

	float rl = right - left;
	float tb = top - bottom;
	float fn = far - near;

	m.m[0][0] = 2.0f / rl;
	m.m[1][1] = 2.0f / tb;
	m.m[2][2] = -2.0f / fn;
	m.m[3][3] = 1.0f;

	m.m[3][0] = -(right + left) / rl;
	m.m[3][1] = -(top + bottom) / tb;
	m.m[3][2] = -(far + near) / fn;

	return m;
}

// glm version
static inline mat4x4_t mat4x4_perspective(float fovy_rad, float aspect, float near, float far) {
	mat4x4_t m = {0};
	float const tan_half_fovy = tanf(fovy_rad / 2.0f);

	m.m[0][0] = 1.0f / (aspect * tan_half_fovy);
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f / (tan_half_fovy);
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = -(far + near) / (far - near);
	m.m[2][3] = -1.0f;

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = -(2.0f * far * near) / (far - near);
	m.m[3][3] = 0.0f;

	return m;
}

static inline mat4x4_t mat4x4_perspective_lh(float fovy_rad, float aspect, float near, float far) {
	mat4x4_t m = {0};
	float const tan_half_fovy = tanf(fovy_rad / 2.0f);

	// Berechnungen für die Perspektivmatrix im Linkshändigen Koordinatensystem
	m.m[0][0] = 1.0f / (aspect * tan_half_fovy);  // X-Achse
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f / (tan_half_fovy);	 // Y-Achse
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = (far + near) / (far - near);  // Linkshändiges Koordinatensystem: Hier müssen wir die Z-Werte umkehren
	m.m[2][3] = 1.0f;						  // Bei Linkshändig, 1.0f für die Z-Werte (während wir -1.0f im rechtshändigen Koordinatensystem hätten)

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = (2.0f * far * near) / (far - near);	 // Linkshändiges Koordinatensystem
	m.m[3][3] = 0.0f;

	return m;
}

static inline mat4x4_t mat4x4_transpose(mat4x4_t m) {
	mat4x4_t out = mat4x4(0);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out.m[i][j] = m.m[j][i];
		}
	}
	return out;
}

static inline mat4x4_t mat4x4_inverse(mat4x4_t m) {
	float* a = (float*)m.m;
	mat4x4_t inv;
	float* o = (float*)inv.m;

	float s0 = a[0] * a[5] - a[4] * a[1];
	float s1 = a[0] * a[6] - a[4] * a[2];
	float s2 = a[0] * a[7] - a[4] * a[3];
	float s3 = a[1] * a[6] - a[5] * a[2];
	float s4 = a[1] * a[7] - a[5] * a[3];
	float s5 = a[2] * a[7] - a[6] * a[3];

	float c5 = a[10] * a[15] - a[14] * a[11];
	float c4 = a[9] * a[15] - a[13] * a[11];
	float c3 = a[9] * a[14] - a[13] * a[10];
	float c2 = a[8] * a[15] - a[12] * a[11];
	float c1 = a[8] * a[14] - a[12] * a[10];
	float c0 = a[8] * a[13] - a[12] * a[9];

	float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;

	if (fabsf(det) < FLT_EPSILON) {
		return mat4x4_identity();
	}

	float inv_det = 1.0f / det;

	o[0] = (a[5] * c5 - a[6] * c4 + a[7] * c3) * inv_det;
	o[1] = (-a[1] * c5 + a[2] * c4 - a[3] * c3) * inv_det;
	o[2] = (a[13] * s5 - a[14] * s4 + a[15] * s3) * inv_det;
	o[3] = (-a[9] * s5 + a[10] * s4 - a[11] * s3) * inv_det;

	o[4] = (-a[4] * c5 + a[6] * c2 - a[7] * c1) * inv_det;
	o[5] = (a[0] * c5 - a[2] * c2 + a[3] * c1) * inv_det;
	o[6] = (-a[12] * s5 + a[14] * s2 - a[15] * s1) * inv_det;
	o[7] = (a[8] * s5 - a[10] * s2 + a[11] * s1) * inv_det;

	o[8] = (a[4] * c4 - a[5] * c2 + a[7] * c0) * inv_det;
	o[9] = (-a[0] * c4 + a[1] * c2 - a[3] * c0) * inv_det;
	o[10] = (a[12] * s4 - a[13] * s2 + a[15] * s0) * inv_det;
	o[11] = (-a[8] * s4 + a[9] * s2 - a[11] * s0) * inv_det;

	o[12] = (-a[4] * c3 + a[5] * c1 - a[6] * c0) * inv_det;
	o[13] = (a[0] * c3 - a[1] * c1 + a[2] * c0) * inv_det;
	o[14] = (-a[12] * s3 + a[13] * s1 - a[14] * s0) * inv_det;
	o[15] = (a[8] * s3 - a[9] * s1 + a[10] * s0) * inv_det;

	return inv;
}
