#ifndef MATH_H
#define MATH_H
#include "types.h"
#include <float.h>//FLT_EPSILON
#include <math.h>//sqrtf
#include <string.h>//memcpy

#define PI 3.14159265358979323846
#define E  2.71828182845904523536

//function templates
//add
#define __vec2_add(type) \
static inline type type ## _add(type a, type b){return (type){{a.x + b.x, a.y + b.y}};}
#define __vec3_add(type) \
static inline type type ## _add(type a, type b){return (type){{a.x + b.x, a.y + b.y, a.z + b.z}};}
#define __vec4_add(type) \
static inline type type ## _add(type a, type b){return (type){{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}};}
//sub
#define __vec2_sub(type) \
static inline type type ## _sub(type a, type b){return (type){{a.x - b.x, a.y - b.y}};}
#define __vec3_sub(type) \
static inline type type ## _sub(type a, type b){return (type){{a.x - b.x, a.y - b.y, a.z - b.z}};}
#define __vec4_sub(type) \
static inline type type ## _sub(type a, type b){return (type){{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}};}
//mul (component wise multiplication)
#define __vec2_mul(type) \
static inline type type ## _mul(type a, type b){return (type){{a.x * b.x, a.y * b.y}};}
#define __vec3_mul(type) \
static inline type type ## _mul(type a, type b){return (type){{a.x * b.x, a.y * b.y, a.z * b.z}};}
#define __vec4_mul(type) \
static inline type type ## _mul(type a, type b){return (type){{a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}};}
//scale
#define __vec2_scale(type) \
static inline type type ## _scale(type a, float b){return (type){{a.x * b, a.y * b}};}
#define __vec3_scale(type) \
static inline type type ## _scale(type a, float b){return (type){{a.x * b, a.y * b, a.z * b}};}
#define __vec4_scale(type) \
static inline type type ## _scale(type a, float b){return (type){{a.x * b, a.y * b, a.z * b, a.w * b}};}
//negate
#define __vec2_negate(type) \
static inline type type ## _negate(type a){return (type){{-a.x, -a.y}};}
#define __vec3_negate(type) \
static inline type type ## _negate(type a){return (type){{-a.x, -a.y, -a.z}};}
#define __vec4_negate(type) \
static inline type type ## _negate(type a){return (type){{-a.x, -a.y, -a.z, -a.w}};}
//lerp
#define __vec2_lerp(type) \
static inline type type ## _lerp(type a, type b, float t){return (type){{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t}};}
#define __vec3_lerp(type) \
static inline type type ## _lerp(type a, type b, float t){return (type){{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t}};}
#define __vec4_lerp(type) \
static inline type type ## _lerp(type a, type b, float t){return (type){{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t}};}
//dot
#define __vec2_dot(type) \
static inline f32 type ## _dot(type a, type b){return a.x*b.x + a.y*b.y;}
#define __vec3_dot(type) \
static inline f32 type ## _dot(type a, type b){return a.x*b.x + a.y*b.y + a.z*b.z;}
#define __vec4_dot(type) \
static inline f32 type ## _dot(type a, type b){return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;}
//length_squared
#define __vec_length_squared(type) \
static inline f32 type ## _length_squared(type a){return type ## _dot(a, a);}
//length
#define __vec_length(type) \
static inline f32 type ## _length(type a){return sqrtf(type ## _length_squared(a));}
//norm
#define __vec_norm(type) \
static inline type type ## _norm(type a){ f32 len = type ## _length(a); if(len > FLT_EPSILON) return type ## _scale(a, 1.0/len); return a;}

//not for every vector:

//for vec2/vec3: distance (between points)
#define __vec_distance(type) \
static inline f32 type ## _distance(type a, type b){return type ## _length( type ## _sub(a, b) );}
//for vec3: cross
#define __vec3_cross(type) \
static inline type type ## _cross(type a, type b){return (type){{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}};}
//generate functions
//add
__vec2_add(vec2s)
__vec2_add(vec2u)
__vec2_add(vec2f)
__vec3_add(vec3s)
__vec3_add(vec3u)
__vec3_add(vec3f)
__vec4_add(vec4s)
__vec4_add(vec4u)
__vec4_add(vec4f)
//sub
__vec2_sub(vec2s)
__vec2_sub(vec2u)
__vec2_sub(vec2f)
__vec3_sub(vec3s)
__vec3_sub(vec3u)
__vec3_sub(vec3f)
__vec4_sub(vec4s)
__vec4_sub(vec4u)
__vec4_sub(vec4f)
//mul
__vec2_mul(vec2s)
__vec2_mul(vec2u)
__vec2_mul(vec2f)
__vec3_mul(vec3s)
__vec3_mul(vec3u)
__vec3_mul(vec3f)
__vec4_mul(vec4s)
__vec4_mul(vec4u)
__vec4_mul(vec4f)
//scale
__vec2_scale(vec2s)
__vec2_scale(vec2u)
__vec2_scale(vec2f)
__vec3_scale(vec3s)
__vec3_scale(vec3u)
__vec3_scale(vec3f)
__vec4_scale(vec4s)
__vec4_scale(vec4u)
__vec4_scale(vec4f)
//negate
__vec2_negate(vec2s)
__vec2_negate(vec2u)
__vec2_negate(vec2f)
__vec3_negate(vec3s)
__vec3_negate(vec3u)
__vec3_negate(vec3f)
__vec4_negate(vec4s)
__vec4_negate(vec4u)
__vec4_negate(vec4f)
//lerp
__vec2_lerp(vec2s)
__vec2_lerp(vec2u)
__vec2_lerp(vec2f)
__vec3_lerp(vec3s)
__vec3_lerp(vec3u)
__vec3_lerp(vec3f)
__vec4_lerp(vec4s)
__vec4_lerp(vec4u)
__vec4_lerp(vec4f)
//dot
__vec2_dot(vec2s)
__vec2_dot(vec2u)
__vec2_dot(vec2f)
__vec3_dot(vec3s)
__vec3_dot(vec3u)
__vec3_dot(vec3f)
__vec4_dot(vec4s)
__vec4_dot(vec4u)
__vec4_dot(vec4f)
//length_squared
__vec_length_squared(vec2s)
__vec_length_squared(vec2u)
__vec_length_squared(vec2f)
__vec_length_squared(vec3s)
__vec_length_squared(vec3u)
__vec_length_squared(vec3f)
__vec_length_squared(vec4s)
__vec_length_squared(vec4u)
__vec_length_squared(vec4f)
//length
__vec_length(vec2s)
__vec_length(vec2u)
__vec_length(vec2f)
__vec_length(vec3s)
__vec_length(vec3u)
__vec_length(vec3f)
__vec_length(vec4s)
__vec_length(vec4u)
__vec_length(vec4f)
//norm
__vec_norm(vec2s)
__vec_norm(vec2u)
__vec_norm(vec2f)
__vec_norm(vec3s)
__vec_norm(vec3u)
__vec_norm(vec3f)
__vec_norm(vec4s)
__vec_norm(vec4u)
__vec_norm(vec4f)
//distance
__vec_distance(vec2s)
__vec_distance(vec2u)
__vec_distance(vec2f)
__vec_distance(vec3s)
__vec_distance(vec3u)
__vec_distance(vec3f)
//cross
__vec3_cross(vec3s)
__vec3_cross(vec3u)
__vec3_cross(vec3f)

//mat4x4

static inline mat4x4 mat4x4_copy_array(float array[16]) {
	mat4x4 m;
	memcpy(m.m16, array, 16 * sizeof(float));
	return m;
}

static inline mat4x4 mat4x4_identity() {
	mat4x4 m = MAT4X4(0);
	m.m16[0] = m.m16[5] = m.m16[10] = m.m16[15] = 1.0f;
	return m;
}

static inline mat4x4 mat4x4_mul(const mat4x4* a, const mat4x4* b) {
	mat4x4 result = MAT4X4(0);
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

static inline vec4f mat4x4_mul_vec4(const mat4x4* m, const vec4f* v) {
	return VEC4F(
		m->m[0][0]*v->x + m->m[1][0]*v->y + m->m[2][0]*v->z + m->m[3][0]*v->w,
		m->m[0][1]*v->x + m->m[1][1]*v->y + m->m[2][1]*v->z + m->m[3][1]*v->w,
		m->m[0][2]*v->x + m->m[1][2]*v->y + m->m[2][2]*v->z + m->m[3][2]*v->w,
		m->m[0][3]*v->x + m->m[1][3]*v->y + m->m[2][3]*v->z + m->m[3][3]*v->w
	);
}

static inline void mat4x4ranslate(mat4x4* out, float x, float y, float z) {
	mat4x4 t = MAT4X4(0);
	t.m[0][0] = 1.0f;
	t.m[1][1] = 1.0f;
	t.m[2][2] = 1.0f;
	t.m[3][3] = 1.0f;
	t.m[3][0] = x;
	t.m[3][1] = y;
	t.m[3][2] = z;

	*out = mat4x4_mul(out, &t);
}

static inline void mat4x4_scale(mat4x4* out, float x, float y, float z) {
	mat4x4 s = MAT4X4(0);
	s.m[0][0] = x;
	s.m[1][1] = y;
	s.m[2][2] = z;
	s.m[3][3] = 1.0f;

	*out = mat4x4_mul(out, &s);
}

//difference too big to glms rotate function
/*static inline void mat4x4_rotate(mat4x4* out, float angle_rad, vec3f axis) {
	axis = vec3f_norm(axis);
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	float t = 1.0f - c;

	float x = axis.x, y = axis.y, z = axis.z;

	mat4x4 r = MAT4X4(0);
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
}*/

static inline mat4x4 mat4x4_lookat(vec3f eye, vec3f center, vec3f up) {
	mat4x4 m = {0};
	vec3f f = vec3f_norm(vec3f_sub(center, eye));
	vec3f s = vec3f_norm(vec3f_cross(f, up));
	vec3f u = vec3f_cross(s, f);

	m.m[0][0] = s.x;
	m.m[0][1] = u.x;
	m.m[0][2] = -f.x;
	m.m[1][0] = s.y;
	m.m[1][1] = u.y;
	m.m[1][2] = -f.y;
	m.m[2][0] = s.z;
	m.m[2][1] = u.z;
	m.m[2][2] = -f.z;
	m.m[3][0] = -vec3f_dot(s, eye);
	m.m[3][1] = -vec3f_dot(u, eye);
	m.m[3][2] = vec3f_dot(f, eye);
	m.m[0][3] = m.m[1][3] = m.m[2][3] = 0.0f;
	m.m[3][3] = 1.0f;

	return m;
}
// left handed
static inline mat4x4 mat4x4_lookat_lh(vec3f eye, vec3f center, vec3f up) {
	mat4x4 m = {0};
	vec3f f = vec3f_norm(vec3f_sub(center, eye));
	vec3f s = vec3f_norm(vec3f_cross(f, up));
	vec3f u = vec3f_cross(s, f);

	m.m[0][0] = s.x;
	m.m[0][1] = u.x;
	m.m[0][2] = f.x;
	m.m[1][0] = s.y;
	m.m[1][1] = u.y;
	m.m[1][2] = f.y;
	m.m[2][0] = s.z;
	m.m[2][1] = u.z;
	m.m[2][2] = f.z;

	m.m[3][0] = -vec3f_dot(s, eye);
	m.m[3][1] = -vec3f_dot(u, eye);
	m.m[3][2] = vec3f_dot(f, eye);
	m.m[0][3] = m.m[1][3] = m.m[2][3] = 0.0f;
	m.m[3][3] = 1.0f;

	return m;
}

static inline mat4x4 mat4x4_ortho(float left, float right, float bottom, float top, float near, float far) {
	mat4x4 m = mat4x4_identity();

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
static inline mat4x4 mat4x4_perspective(float fovy_rad, float aspect, float near, float far) {
	mat4x4 m = {0};
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

static inline mat4x4 mat4x4_perspective_lh(float fovy_rad, float aspect, float near, float far) {
	mat4x4 m = {0};
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

static inline mat4x4 mat4x4_transpose(mat4x4 m) {
	mat4x4 out = MAT4X4(0);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out.m[i][j] = m.m[j][i];
		}
	}
	return out;
}

static inline mat4x4 mat4x4_inverse(mat4x4 m) {
	float* a = (float*)m.m;
	mat4x4 inv;
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


//complex
//add
static inline complexf complexf_add(complexf a, complexf b){
	return (complexf){a.re + b.re, a.im + b.im)};
}
//sub
static inline complexf complexf_sub(complexf a, complexf b){
	return (complexf){a.re - b.re, a.im + b.im)};
}
//mul
/*
	 => (a + bi) * (c + di)
	<=>  ac + adi + bci - bd
	<=>  (ac - bd) + i(ad + bc)
*/
static inline complexf complexf_mul(complexf a, complexf b){
	return (complexf){a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re)};
}
//scale
static inline complexf complexf_scale(complexf a, f64 scale){
	return (complexf){a.re*scale, a.im*scale)};
}
//cartesian complex from euler
static inline complexf complexf_from_euler(f64 magnitude, f64 argument){
	return (complexf){magnitude * cosf(argument), magnitude * sinf(argument)};
}
//complex conjugate
static inline complexf complexf_conjugate(complexf c){
	return (complexf){c.re, -c.im};
}
//absolute value / magnitude
static inline f64 complexf_abs(complexf c){
	return sqrtf(c.re * c.re + c.im * c.im);
}
//argument
static inline f64 complexf_arg(complexf c){
	if(c.re > 0 && c.im == 0) return PI;
	return 2 * atanf(c.im/(complexf_abs(c) + c.re));
}
#endif
