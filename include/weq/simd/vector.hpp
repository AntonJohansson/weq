#pragma once

#include "Defines.hpp"
//#include <xmmintrin.h>
//#include <stdint.h>
//#include <cstdio>
//#include <cstdlib>

#define MATH_SHUFFLE3(a, b, x, y, z)    _mm_shuffle_ps(a, b, _MM_SHUFFLE(3, z, y, x))
#define MATH_SHUFFLE4(a, b, x, y, z, w) _mm_shuffle_ps(a, b, _MM_SHUFFLE(w, z, y, x))

// NOTE:
// v3 and v4 are identical only that v3 zeroes the last element of v4
// should be able to effectively half the size of this code.
//
// todo: 	- move const vecs here
// 				- m4,m5,q4

namespace simd{

struct v3{
	FORCE_INLINE v3(){}
	FORCE_INLINE explicit v3(float x, float y, float z){m = _mm_set_ps(0, z, y, x);}
	FORCE_INLINE explicit v3(float a){m = _mm_set_ps(0, a, a, a);}
	FORCE_INLINE explicit v3(float* a){m = _mm_set_ps(0, a[2], a[1], a[0]);}
	FORCE_INLINE explicit v3(__m128 v){m = v;}
	FORCE_INLINE operator __m128(){return m;}
	FORCE_INLINE operator __m128() const {return m;}

	__m128 m;
};

struct v4{
	FORCE_INLINE v4(){}
	FORCE_INLINE explicit v4(float x, float y, float z, float w){m = _mm_set_ps(w, z, y, x);}
	FORCE_INLINE explicit v4(float a){m = _mm_set_ps(a, a, a, a);}
	FORCE_INLINE explicit v4(float* a){m = _mm_set_ps(a[3], a[2], a[1], a[0]);}
	FORCE_INLINE explicit v4(__m128 v){m = v;}
	FORCE_INLINE operator __m128(){return m;}
	FORCE_INLINE operator __m128() const {return m;}

	__m128 m;
};

struct vfloat{
	FORCE_INLINE vfloat(){}
	FORCE_INLINE explicit vfloat(float a)  {m = _mm_set_ps(0, a, a, a);}
	FORCE_INLINE explicit vfloat(v3& a)    {m = _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 0, 0, 0));}
	FORCE_INLINE explicit vfloat(v4& a)    {m = _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 0, 0, 0));}
	FORCE_INLINE explicit vfloat(__m128 v) {m = v;}
	FORCE_INLINE operator __m128()       {return m;}
	FORCE_INLINE operator __m128() const {return m;}

	float to_float()       {return _mm_cvtss_f32(m);}
	float to_float() const {return _mm_cvtss_f32(m);}

	operator float()       {return to_float();}
	operator float() const {return to_float();}

	__m128 m;
};

// Helper structs to enable static construction of v3/v4
struct v3_consti
{
	union {uint32_t i[4]; __m128 m;};
	FORCE_INLINE operator v3()     const {return v3(m);}
	FORCE_INLINE operator __m128() const {return m;}
};

struct v3_constf
{
	union {float f[4]; __m128 m;};
	FORCE_INLINE operator v3()     const {return v3(m);}
	FORCE_INLINE operator __m128() const {return m;}
};

struct v4_consti
{
	union {uint32_t i[4]; __m128 m;};
	FORCE_INLINE operator v4()     const {return v4(m);}
	FORCE_INLINE operator __m128() const {return m;}
};

struct v4_constf
{
	union {float f[4]; __m128 m;};
	FORCE_INLINE operator v4()     const {return v4(m);}
	FORCE_INLINE operator __m128() const {return m;}
};

// Accessing
FORCE_INLINE vfloat get_x(v3 a) {return vfloat(MATH_SHUFFLE3(a, a, 0, 0, 0));}
FORCE_INLINE vfloat get_y(v3 a) {return vfloat(MATH_SHUFFLE3(a, a, 1, 1, 1));}
FORCE_INLINE vfloat get_z(v3 a) {return vfloat(MATH_SHUFFLE3(a, a, 2, 2, 2));}

FORCE_INLINE vfloat get_x(v4 a) {return vfloat(MATH_SHUFFLE4(a, a, 0, 0, 0, 0));}
FORCE_INLINE vfloat get_y(v4 a) {return vfloat(MATH_SHUFFLE4(a, a, 1, 1, 1, 1));}
FORCE_INLINE vfloat get_z(v4 a) {return vfloat(MATH_SHUFFLE4(a, a, 2, 2, 2, 2));}
FORCE_INLINE vfloat get_w(v4 a) {return vfloat(MATH_SHUFFLE4(a, a, 3, 3, 3, 3));}

FORCE_INLINE v3 splat_x(v3 a) {return v3(MATH_SHUFFLE3(a, a, 0, 0, 0));}
FORCE_INLINE v3 splat_y(v3 a) {return v3(MATH_SHUFFLE3(a, a, 1, 1, 1));}
FORCE_INLINE v3 splat_z(v3 a) {return v3(MATH_SHUFFLE3(a, a, 2, 2, 2));}

FORCE_INLINE v4 splat_x(v4 a) {return v4(MATH_SHUFFLE4(a, a, 0, 0, 0, 0));}
FORCE_INLINE v4 splat_y(v4 a) {return v4(MATH_SHUFFLE4(a, a, 1, 1, 1, 1));}
FORCE_INLINE v4 splat_z(v4 a) {return v4(MATH_SHUFFLE4(a, a, 2, 2, 2, 2));}
FORCE_INLINE v4 splat_w(v4 a) {return v4(MATH_SHUFFLE4(a, a, 3, 3, 3, 3));}

// f must be 16 byte aligned
// TODO: cannot redeclare load for v4 since it is ambiguating
FORCE_INLINE v3   load(float* f)        {return v3(_mm_load_ps(f));}
FORCE_INLINE void store(v3 v, float* f) {_mm_store_ps(f, v);}
//FORCE_INLINE v4   load(float* f)        {return v4(_mm_load_ps(f));}
FORCE_INLINE void store(v4 v, float* f) {_mm_store_ps(f, v);}

// v3
FORCE_INLINE v3 set_x(v3 a, float x){
	v3 t0 = v3(_mm_set_ss(x));
	return v3(_mm_move_ss(a, t0));
}

FORCE_INLINE v3 set_y(v3 a, float y){
	v3 t0 = v3(MATH_SHUFFLE3(a, a, 1, 0, 2));
	v3 t1 = v3(_mm_set_ss(y));
	v3 t2 = v3(_mm_move_ss(t0, t1));
	return v3(MATH_SHUFFLE3(t2, t2, 1, 0, 2));
}

FORCE_INLINE v3 set_z(v3 a, float z){
	v3 t0 = v3(MATH_SHUFFLE3(a, a, 2, 1, 0));
	v3 t1 = v3(_mm_set_ss(z));
	v3 t2 = v3(_mm_move_ss(t0, t1));
	return v3(MATH_SHUFFLE3(t2, t2, 2, 1, 0));
}

// v4
FORCE_INLINE v4 set_x(v4 a, float x){
	v4 t0 = v4(_mm_set_ss(x));
	return v4(_mm_move_ss(a, t0));
}

// [1, 2, 3, 4]
// t0 = shuffle -> [2, 1, 3, 4]
// t1 = [5, 0, 0, 0]
// t2 = move -> [5, 1, 3, 4]
// res = shufle -> [1, 5, 3, 4]
FORCE_INLINE v4 set_y(v4 a, float y){
	v4 t0 = v4(MATH_SHUFFLE4(a, a, 1, 0, 2, 3));
	v4 t1 = v4(_mm_set_ss(y));
	v4 t2 = v4(_mm_move_ss(t0, t1));
	return v4(MATH_SHUFFLE4(t2, t2, 1, 0, 2, 3));
}

// [1, 2, 3, 4]
// t0 = shuffle -> [3, 2, 1, 4]
// t1 = [5, 0, 0, 0]
// t2 = move -> [5, 2, 1, 4]
// res = shufle -> [1, 2, 5, 4]
FORCE_INLINE v4 set_z(v4 a, float z){
	v4 t0 = v4(MATH_SHUFFLE4(a, a, 2, 1, 0, 3));
	v4 t1 = v4(_mm_set_ss(z));
	v4 t2 = v4(_mm_move_ss(t0, t1));
	return v4(MATH_SHUFFLE4(t2, t2, 2, 1, 0, 3));
}

// [1, 2, 3, 4]
// t0 = shuffle -> [4, 2, 3, 1]
// t1 = [5, 0, 0, 0]
// t2 = move -> [5, 2, 3, 1]
// res = shufle -> [1, 2, 3, 5]
FORCE_INLINE v4 set_w(v4 a, float w){
	v4 t0 = v4(MATH_SHUFFLE4(a, a, 3, 1, 2, 0));
	v4 t1 = v4(_mm_set_ss(w));
	v4 t2 = v4(_mm_move_ss(t0, t1));
	return v4(MATH_SHUFFLE4(t2, t2, 3, 1, 2, 0));
}

// Useful functions
// TODO: allocators
FORCE_INLINE const char* to_string(v3 v){
	constexpr int whole_precision   = 4;
	constexpr int decimal_precision = 2;
	constexpr int constant_bytes    = 6;// (, , )
	constexpr int size = whole_precision*decimal_precision*3 + constant_bytes;

	char* buffer = static_cast<char*>(malloc(size*sizeof(char) + 1));

	std::snprintf(buffer, size, "(%7.*f, %7.*f, %7.*f)",
			decimal_precision, decimal_precision, decimal_precision,
			(float)get_x(v), (float)get_y(v), (float)get_z(v));
	
	return buffer;
}

FORCE_INLINE const char* to_string(v4 v){
	constexpr int whole_precision   = 4;
	constexpr int decimal_precision = 2;
	constexpr int constant_bytes    = 8;// (, , , )
	constexpr int size = whole_precision*decimal_precision*4 + constant_bytes + 10;

	char* buffer = static_cast<char*>(malloc(size*sizeof(char) + 1));

	std::snprintf(buffer, size, "(%7.2f, %7.2f, %7.2f, %7.2f)",
			(float)get_x(v), (float)get_y(v), (float)get_z(v), (float)get_w(v));
	
	return buffer;
}

// vector opererations

// a, b are n-dimensional vectors
// a . b = a[0]*b[0] + ... + a[n]*b[n]
FORCE_INLINE vfloat dot(v3 a, v3 b){
	// a = [1,2,3]
	// b = [4,5,6]
	// a.b = 4 + 10 + 18 = 32
	
	// t0 = [1*4, 2*5, 3*6] = [4, 10, 18]
	v3 t0 = v3(_mm_mul_ps(a, b));
	// t1 = [10, 4, 4]
	v3 t1 = v3(MATH_SHUFFLE3(t0, t0, 1, 0, 0));
	// t2 = [4+10, 10 18] = [14, 10, 18]
	v3 t2 = v3(_mm_add_ss(t0, t1));
	// t3 = [18, 14, 14]
	v3 t3 = v3(MATH_SHUFFLE3(t2, t2, 2, 0, 0));
	// t4 = [14+18, 10, 18] = [32, 14, 22]
	v3 t4 = v3(_mm_add_ss(t2, t3));
	return vfloat(t4);
}

// a, b are n-dimensional vectors
// a . b = a[0]*b[0] + ... + a[n]*b[n]
FORCE_INLINE vfloat dot(v4 a, v4 b){
	v4 t0 = v4(_mm_mul_ps(a, b));
	v4 t1 = v4(MATH_SHUFFLE4(t0, t0, 1, 0, 0, 0));
	v4 t2 = v4(_mm_add_ss(t0, t1));
	v4 t3 = v4(MATH_SHUFFLE4(t2, t2, 2, 0, 0, 0));
	v4 t4 = v4(_mm_add_ss(t2, t3));
	v4 t5 = v4(MATH_SHUFFLE4(t4, t4, 3, 0, 0, 0));
	v4 t6 = v4(_mm_add_ss(t4, t5));
	return vfloat(t6);
}

// a = [1, 2, 3]
// b = [4, 5, 6]
//
//         |x y z|
// a x b = |1 2 3| =  [2*6 - 3*5, 3*4 - 1*6, 1*5 - 2*4] = [-3, 6, -3]
//         |4 5 6|
FORCE_INLINE v3 cross(v3 a, v3 b){
	// the following three operations gives the positive terms
	// in each component.
	
	// t0 = [2, 3, 1]
	v3 t0 = v3(MATH_SHUFFLE3(a, a, 1, 2, 0));
	// t1 = [6, 4, 5]
	v3 t1 = v3(MATH_SHUFFLE3(b, b, 2, 0, 1));
	// t2 = [2*6, 3*4, 1*5] = [12, 12, 5]
	v3 t2 = v3(_mm_mul_ps(t0, t1));

	// the following three operations gives the negative terms
	// in each component.

	// t0 = [3, 1, 2]
	t0 = v3(MATH_SHUFFLE3(t0, t0, 1, 2, 0));
	// t1 = [5, 6, 4]
	t1 = v3(MATH_SHUFFLE3(t1, t1, 2, 0, 1));
	// t0 = [5*3, 1*6, 2*4] = [15, 6, 8]
	t0 = v3(_mm_mul_ps(t0, t1));

	// [12 - 15, 12 - 6, 5 - 8] = [-3, 6, -3]
	return v3(_mm_sub_ps(t2, t0));
}

WEAK_SYMBOL v3_consti v3_mask_sign = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
WEAK_SYMBOL v3_consti v3_mask_all  = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000};
WEAK_SYMBOL v4_consti v4_mask_sign = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
WEAK_SYMBOL v4_consti v4_mask_all  = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

FORCE_INLINE vfloat length_sq(v3 a){return dot(a,a);}
FORCE_INLINE vfloat length_sq(v4 a){return dot(a,a);}
FORCE_INLINE vfloat sqrt(vfloat a) {return vfloat(_mm_sqrt_ps(a));}
FORCE_INLINE vfloat length(v3 a)   {return sqrt(length_sq(a));}
FORCE_INLINE vfloat length(v4 a)   {return sqrt(length_sq(a));}

FORCE_INLINE v3 normalize(v3 a){
	vfloat t0 = length(a);
	v3 t1 = v3(_mm_div_ps(a, t0));
	return v3(_mm_and_ps(t1, v3_mask_all));
}

FORCE_INLINE v4 normalize(v4 a){
	vfloat t0 = length(a);
	v4 t1 = v4(_mm_div_ps(a, t0));
	return v4(_mm_and_ps(t1, v4_mask_all));
}

// the binary ops
FORCE_INLINE v3  operator+ (v3  a, v3 b) {return v3( _mm_add_ps(a, b) );}
FORCE_INLINE v3  operator- (v3  a, v3 b) {return v3( _mm_sub_ps(a, b) );}
FORCE_INLINE v3& operator+=(v3& a, v3 b) {a = a + b; return a;}
FORCE_INLINE v3& operator-=(v3& a, v3 b) {a = a - b; return a;}

FORCE_INLINE v4  operator+ (v4  a, v4 b) {return v4( _mm_add_ps(a, b) );}
FORCE_INLINE v4  operator- (v4  a, v4 b) {return v4( _mm_sub_ps(a, b) );}
FORCE_INLINE v4& operator+=(v4& a, v4 b) {a = a + b; return a;}
FORCE_INLINE v4& operator-=(v4& a, v4 b) {a = a - b; return a;}


FORCE_INLINE vfloat  operator+ (vfloat  a, vfloat b) {return vfloat( _mm_add_ps( a, b ) );}
FORCE_INLINE vfloat  operator- (vfloat  a, vfloat b) {return vfloat( _mm_sub_ps( a, b ) );}
FORCE_INLINE vfloat  operator* (vfloat  a, vfloat b) {return vfloat( _mm_mul_ps( a, b ) );}
FORCE_INLINE vfloat  operator/ (vfloat  a, vfloat b) {return vfloat( _mm_div_ps( a, b ) );}
FORCE_INLINE vfloat& operator+=(vfloat& a, vfloat b) {a = a + b; return a;}
FORCE_INLINE vfloat& operator-=(vfloat& a, vfloat b) {a = a - b; return a;}
FORCE_INLINE vfloat& operator*=(vfloat& a, vfloat b) {a = a * b; return a;}
FORCE_INLINE vfloat& operator/=(vfloat& a, vfloat b) {a = a / b; return a;}

FORCE_INLINE vfloat  operator+ (vfloat  a, float b) {return vfloat(_mm_add_ps(a, vfloat(b)));}
FORCE_INLINE vfloat  operator- (vfloat  a, float b) {return vfloat(_mm_sub_ps(a, vfloat(b)));}
FORCE_INLINE vfloat  operator* (vfloat  a, float b) {return vfloat(_mm_mul_ps(a, vfloat(b)));}
FORCE_INLINE vfloat  operator/ (vfloat  a, float b) {return vfloat(_mm_div_ps(a, vfloat(b)));}
FORCE_INLINE vfloat& operator+=(vfloat& a, float b) {a = a + b; return a;}
FORCE_INLINE vfloat& operator-=(vfloat& a, float b) {a = a - b; return a;}
FORCE_INLINE vfloat& operator*=(vfloat& a, float b) {a = a * b; return a;}
FORCE_INLINE vfloat& operator/=(vfloat& a, float b) {a = a / b; return a;}

FORCE_INLINE vfloat operator+ (float  a, vfloat b) {return vfloat(_mm_add_ps(vfloat(a), b));}
FORCE_INLINE vfloat operator- (float  a, vfloat b) {return vfloat(_mm_sub_ps(vfloat(a), b));}
FORCE_INLINE vfloat operator* (float  a, vfloat b) {return vfloat(_mm_mul_ps(vfloat(a), b));}
FORCE_INLINE vfloat operator/ (float  a, vfloat b) {return vfloat(_mm_div_ps(vfloat(a), b));}
FORCE_INLINE float& operator+=(float& a, vfloat b) {a = a + b; return a;}
FORCE_INLINE float& operator-=(float& a, vfloat b) {a = a - b; return a;}
FORCE_INLINE float& operator*=(float& a, vfloat b) {a = a * b; return a;}
FORCE_INLINE float& operator/=(float& a, vfloat b) {a = a / b; return a;}

// Comparisons
// generally comparisons are followed up with a mask(v3) call (or any(v3))
FORCE_INLINE v3 operator==(v3 a, v3 b) {return v3(_mm_cmpeq_ps(a, b));}
FORCE_INLINE v3 operator!=(v3 a, v3 b) {return v3(_mm_cmpneq_ps(a, b));}
FORCE_INLINE v3 operator< (v3 a, v3 b) {return v3(_mm_cmplt_ps(a, b));}
FORCE_INLINE v3 operator> (v3 a, v3 b) {return v3(_mm_cmpgt_ps(a, b));}
FORCE_INLINE v3 operator<=(v3 a, v3 b) {return v3(_mm_cmple_ps(a, b));}
FORCE_INLINE v3 operator>=(v3 a, v3 b) {return v3(_mm_cmpge_ps(a, b));}
FORCE_INLINE v3 operator- (v3 a)       {return v3(_mm_setzero_ps()) - a;}

FORCE_INLINE v4 operator==(v4 a, v4 b) {return v4(_mm_cmpeq_ps(a, b));}
FORCE_INLINE v4 operator!=(v4 a, v4 b) {return v4(_mm_cmpneq_ps(a, b));}
FORCE_INLINE v4 operator< (v4 a, v4 b) {return v4(_mm_cmplt_ps(a, b));}
FORCE_INLINE v4 operator> (v4 a, v4 b) {return v4(_mm_cmpgt_ps(a, b));}
FORCE_INLINE v4 operator<=(v4 a, v4 b) {return v4(_mm_cmple_ps(a, b));}
FORCE_INLINE v4 operator>=(v4 a, v4 b) {return v4(_mm_cmpge_ps(a, b));}
FORCE_INLINE v4 operator- (v4 a)       {return v4(_mm_setzero_ps()) - a;}


FORCE_INLINE vfloat operator==(vfloat a, vfloat b) {return vfloat(_mm_cmpeq_ps(a, b));}
FORCE_INLINE vfloat operator!=(vfloat a, vfloat b) {return vfloat(_mm_cmpneq_ps(a, b));}
FORCE_INLINE vfloat operator< (vfloat a, vfloat b) {return vfloat(_mm_cmplt_ps(a, b));}
FORCE_INLINE vfloat operator> (vfloat a, vfloat b) {return vfloat(_mm_cmpgt_ps(a, b));}
FORCE_INLINE vfloat operator<=(vfloat a, vfloat b) {return vfloat(_mm_cmple_ps(a, b));}
FORCE_INLINE vfloat operator>=(vfloat a, vfloat b) {return vfloat(_mm_cmpge_ps(a, b));}
FORCE_INLINE vfloat operator- (vfloat a)           {return vfloat(_mm_setzero_ps()) - a;}

// Masks
FORCE_INLINE unsigned mask(v3 a) {return _mm_movemask_ps(a) & 7;}
FORCE_INLINE int      any (v3 a) {return mask(a) != 0;}
FORCE_INLINE int      all (v3 a) {return mask(a) == 7;}

FORCE_INLINE unsigned mask(v4 a) {return _mm_movemask_ps(a) & 7;}
FORCE_INLINE int      any (v4 a) {return mask(a) != 0;}
FORCE_INLINE int      all (v4 a) {return mask(a) == 7;}

FORCE_INLINE unsigned mask(vfloat a) {return _mm_movemask_ps(a) & 7;}
FORCE_INLINE int      any (vfloat a) { return mask(a) != 0;}
FORCE_INLINE int      all (vfloat a) { return mask(a) == 7;}

// Vector float operations
FORCE_INLINE v3  operator* (v3  a, v3 b)     {return v3(_mm_mul_ps(a, b));}
FORCE_INLINE v3  operator/ (v3  a, v3 b)     {return v3(_mm_div_ps(a, b));}
FORCE_INLINE v3& operator*=(v3& a, v3 b)     {a = a * b; return a;}
FORCE_INLINE v3& operator/=(v3& a, v3 b)     {a = a / b; return a;}
FORCE_INLINE v3  operator* (v3  a, vfloat b) {return v3(_mm_mul_ps(a, b));}
FORCE_INLINE v3  operator/ (v3  a, vfloat b) {return v3(_mm_div_ps(a, b));}
FORCE_INLINE v3& operator*=(v3& a, vfloat b) {a = a * b; return a;}
FORCE_INLINE v3& operator/=(v3& a, vfloat b) {a = a / b; return a;}
FORCE_INLINE v3  operator* (v3  a, float b)  {return v3(_mm_mul_ps(a, vfloat(b)));}
FORCE_INLINE v3  operator/ (v3  a, float b)  {return v3(_mm_div_ps(a, vfloat(b)));}
FORCE_INLINE v3& operator*=(v3& a, float b)  {a = a * b; return a;}
FORCE_INLINE v3& operator/=(v3& a, float b)  {a = a / b; return a;}

FORCE_INLINE v4  operator* (v4  a, v4 b)     {return v4(_mm_mul_ps(a, b));}
FORCE_INLINE v4  operator/ (v4  a, v4 b)     {return v4(_mm_div_ps(a, b));}
FORCE_INLINE v4& operator*=(v4& a, v4 b)     {a = a * b; return a;}
FORCE_INLINE v4& operator/=(v4& a, v4 b)     {a = a / b; return a;}
FORCE_INLINE v4  operator* (v4  a, vfloat b) {return v4(_mm_mul_ps(a, b));}
FORCE_INLINE v4  operator/ (v4  a, vfloat b) {return v4(_mm_div_ps(a, b));}
FORCE_INLINE v4& operator*=(v4& a, vfloat b) {a = a * b; return a;}
FORCE_INLINE v4& operator/=(v4& a, vfloat b) {a = a / b; return a;}
FORCE_INLINE v4  operator* (v4  a, float b)  {return v4(_mm_mul_ps(a, vfloat(b)));}
FORCE_INLINE v4  operator/ (v4  a, float b)  {return v4(_mm_div_ps(a, vfloat(b)));}
FORCE_INLINE v4& operator*=(v4& a, float b)  {a = a * b; return a;}
FORCE_INLINE v4& operator/=(v4& a, float b)  {a = a / b; return a;}

}
