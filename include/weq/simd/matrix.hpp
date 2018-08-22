#pragma once

#include "Defines.hpp"
#include <xmmintrin.h>
#include <pmmintrin.h> // needed for hadd in inverse(...)
#include <smmintrin.h> // needed for blendv in transform_inverse(...)
#include "vector.hpp"
#include <cmath>
#include <cassert>

#define MATH_SHUFFLE3(a, b, x, y, z)    _mm_shuffle_ps(a, b, _MM_SHUFFLE(3, z, y, x))
#define MATH_SHUFFLE4(a, b, x, y, z, w) _mm_shuffle_ps(a, b, _MM_SHUFFLE(w, z, y, x))

namespace simd{

// MATRICES ARE COLUMN MAJOR!
struct m4;

struct m3{
	FORCE_INLINE v3 operator[](int i){
		switch(i){
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default: assert(0); return x;
		}
	}

	v3 x;
	v3 y;
	v3 z;
};

struct m4{
	FORCE_INLINE v4 operator[](int i){
		switch(i){
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			default: assert(0); return x;
		}
	}

	v4 x;
	v4 y;
	v4 z;
	v4 w;
};

// Useful functions
// TODO: allocators
const char* to_string(m3 a){
	constexpr int decimals = 2;
	constexpr int width = 7;
	constexpr int constant_bytes = 20;
	constexpr int size = 7*16 + constant_bytes;
	char* buffer = static_cast<char*>(malloc(size*sizeof(char) + 1));

	std::snprintf(buffer, size, 
			"%7.2f %7.2f %7.2f\n%7.2f %7.2f %7.2f\n%7.2f %7.2f %7.2f",
			(float)get_x(a.x), (float)get_x(a.y), (float)get_x(a.z),
			(float)get_y(a.x), (float)get_y(a.y), (float)get_y(a.z),
			(float)get_z(a.x), (float)get_z(a.y), (float)get_z(a.z));
	
	return buffer;
}

const char* to_string(m4 a){
	constexpr int decimals = 2;
	constexpr int width = 7;
	constexpr int constant_bytes = 20;
	constexpr int size = 7*16 + constant_bytes;
	char* buffer = static_cast<char*>(malloc(size*sizeof(char) + 1));

	std::snprintf(buffer, size, 
			"%7.2f %7.2f %7.2f %7.2f\n%7.2f %7.2f %7.2f %7.2f\n%7.2f %7.2f %7.2f %7.2f\n%7.2f %7.2f %7.2f %7.2f",
			(float)get_x(a.x), (float)get_x(a.y), (float)get_x(a.z), (float)get_x(a.w),
			(float)get_y(a.x), (float)get_y(a.y), (float)get_y(a.z), (float)get_y(a.w),
			(float)get_z(a.x), (float)get_z(a.y), (float)get_z(a.z), (float)get_z(a.w),
			(float)get_w(a.x), (float)get_w(a.y), (float)get_w(a.z), (float)get_w(a.w));
	
	return buffer;
}

// Operations

FORCE_INLINE void m3_from_m4(){
}

FORCE_INLINE m3 m3_from_cols(v3 x, v3 y, v3 z){
	m3 m;
	m.x = x;
	m.y = y;
	m.z = z;
	return m;
}

FORCE_INLINE m4 m4_from_cols(v4 x, v4 y, v4 z, v4 w){
	m4 m;
	m.x = x;
	m.y = y;
	m.z = z;
	m.w = w;
	return m;
}

m4 transpose(m4);
FORCE_INLINE m4 m4_from_rows(v4 x, v4 y, v4 z, v4 w){
	return transpose(m4_from_rows(x, y, z, w));
}

FORCE_INLINE m4 m4_from_quat(vfloat x, vfloat y, vfloat z, vfloat w){
	vfloat x2 = x + x;
	vfloat y2 = y + y;
	vfloat z2 = z + z;

	vfloat xx = x * x2;
	vfloat xy = x * y2;
	vfloat xz = x * z2;
	vfloat xw = w * x2;
	vfloat yy = y * y2;
	vfloat yz = y * z2;
	vfloat yw = w * y2;
	vfloat zz = z * z2;
	vfloat zw = w * z2;

	vfloat one = vfloat(1.0f);

	//return m4_from_cols(
	//		v4(one - yy - zz, xy + zw, xz - yw, 0),
	//		v4(xy - zw, one - xx - zz, yz + xw, 0),
	//		v4(xz + yw, yz - xw, one - xx - yy, 0),
	//		v4(xz + yw, yz - xw, one - xx - yy, 1));
	return m4_from_cols(
			v4(one - yy - zz, xy + zw,       xz - yw,       0),
			v4(xy - zw,       one - xx - zz, yz + xw,       0),
			v4(xz + yw,       yz - xw,       one - xx - yy, 0),
			v4(0,             0,             0,             1));

}

// Does not preserve 0.0f in w to get rid of extra shuffles.
FORCE_INLINE m3 transpose(m3 a){
	//     x y z w        x y z w
	//     1 4 7 0        1 2 3 0 
	// a = 2 5 8 0  a^T = 4 5 6 0
	//     3 6 9 0        7 8 9 0
	//     0 0 0 0        0 0 0 0
	
	v3 t0 = v3(MATH_SHUFFLE4(a.x, a.y, 0, 1, 0, 1)); // 1 2 4 5
	v3 t1 = v3(MATH_SHUFFLE4(a.x, a.y, 2, 3, 2, 3)); // 3 0 6 0
	v3 x  = v3(MATH_SHUFFLE4(t0, a.z,  0, 2, 0, 3)); // 1 4 7 0
	v3 y  = v3(MATH_SHUFFLE4(t0, a.z,  1, 3, 1, 3)); // 2 5 8 0
	v3 z  = v3(MATH_SHUFFLE4(t1, a.z,  0, 2, 2, 3)); // 3 6 9 0

	a.x = x;
	a.y = y;
	a.z = z;

	return a;
}

FORCE_INLINE m4 transpose(m4 a){
	// https://fgiesen.wordpress.com/2013/07/09/simd-transposes-1/
	// 	where interleave32_lhs = _mm_unpacklo_ps,
	// 				interleave32_rhs = _mm_unpackhi_ps,
	// 				interleave64_lhs = _mm_movelh_ps
	// 				interleave64_rhs = _mm_movehl_ps
	v4 a0 = v4(_mm_unpacklo_ps(a.x, a.y));
	v4 a1 = v4(_mm_unpackhi_ps(a.x, a.y));
	v4 a2 = v4(_mm_unpacklo_ps(a.z, a.w));
	v4 a3 = v4(_mm_unpackhi_ps(a.z, a.w));

	a.x = v4(_mm_movelh_ps(a0, a2));
	a.y = v4(_mm_movehl_ps(a2, a0));
	a.z = v4(_mm_movelh_ps(a1, a3));
	a.w = v4(_mm_movehl_ps(a3, a1));

	return a;
}

FORCE_INLINE v3 mul(m3 a, v3 b){
	//		 a d g   x
	//     b e h x y
	//		 c f i   z
	v3 x = splat_x(b);            // [x x x x]
	v3 y = splat_y(b);            // [y y y y]
	v3 z = splat_z(b);            // [z z z z]
	x = v3(_mm_mul_ps(x, a.x));   // [a b c]x[x x x]
	y = v3(_mm_mul_ps(y, a.y));   // [d e f]x[y y y]
	z = v3(_mm_mul_ps(z, a.z));   // [g h i]x[z z z]
	//
	v3 t0 = v3(_mm_add_ps(x, y));
	return v3( _mm_add_ps(t0, z));
}

FORCE_INLINE v4 mul(m4 a, v4 b){
	//		 a e i m   x
	//     b f j n x y
	//		 c g k o  z
	//		 d h l p
	v4 x = splat_x(b);            // [x x x x]
	v4 y = splat_y(b);            // [y y y y]
	v4 z = splat_z(b);            // [z z z z]
	v4 w = splat_w(b);            // [w w w w]
	x = v4(_mm_mul_ps(x, a.x));   // [a b c d]x[x x x x]
	y = v4(_mm_mul_ps(y, a.y));   // [e f g h]x[y y y y]
	z = v4(_mm_mul_ps(z, a.z));   // [i j k l]x[z z z z]
	w = v4(_mm_mul_ps(w, a.w));   // [m n o p]x[w w w w]
	//
	v4 t0 = v4(_mm_add_ps(x, y));
	v4 t1 = v4(_mm_add_ps(t0, z));
	return v4(_mm_add_ps(t1, w));
}

FORCE_INLINE m3 mul(m3 a, m3 b){
	v3 x = mul(a, b.x);
	v3 y = mul(a, b.y);
	v3 z = mul(a, b.z);
	return m3_from_cols(x, y, z);
}

FORCE_INLINE m4 mul(m4 a, m4 b){
	v4 x = mul(a, b.x);
	v4 y = mul(a, b.y);
	v4 z = mul(a, b.z);
	v4 w = mul(a, b.w);
	return m4_from_cols(x, y, z, w);
}

// MORE COMPLEX OPERATIONS

FORCE_INLINE m4 translate(v3 t){
	// need to get last component to 1
	v4 t1 = v4(t) + v4(0,0,0,1);
	return m4_from_cols(v4(1,0,0,0), v4(0,1,0,0), v4(0,0,1,0), t1);
}
FORCE_INLINE m4 scale(v3 s){
	v4 x(MATH_SHUFFLE4(s, s, 0,3,3,3));
	v4 y(MATH_SHUFFLE4(s, s, 3,1,3,3));
	v4 z(MATH_SHUFFLE4(s, s, 3,3,2,3));
	return m4_from_cols(x, y, z, v4(0,0,0,1));
}


FORCE_INLINE m4 axis_angle(v3 axis, vfloat angle){
	vfloat s = vfloat(std::sin(angle * 0.5f));
	vfloat c = vfloat(std::cos(angle * 0.5f));

	vfloat x = get_x(axis) * s;
	vfloat y = get_y(axis) * s;
	vfloat z = get_z(axis) * s;
	vfloat w = c;

	return m4_from_quat(x, y, z, w);
}

FORCE_INLINE m4 axis_angle(v3 axis, float angle){
	return axis_angle(axis, vfloat(angle));
}

FORCE_INLINE m4 transform_inverse_no_scale(m4 m){
	// this is the code to transpose a 3x3 matrix
	m4 r;
	// 3x3 transpose
	v4 t0 = v4(MATH_SHUFFLE4(m.x, m.y, 0, 1, 0, 1)); // 1 2 4 5
	v4 t1 = v4(MATH_SHUFFLE4(m.x, m.y, 2, 3, 2, 3)); // 3 0 6 0
	r.x   = v4(MATH_SHUFFLE4(t0, m.z,  0, 2, 0, 3)); // 1 4 7 0
	r.y   = v4(MATH_SHUFFLE4(t0, m.z,  1, 3, 1, 3)); // 2 5 8 0
	r.z   = v4(MATH_SHUFFLE4(t1, m.z,  0, 2, 2, 3)); // 3 6 9 0

	// WHAT?
	r.w =       r.x*v4(splat_x(m.w));
	r.w = r.w + r.y*v4(splat_y(m.w));
	r.w = r.w + r.z*v4(splat_z(m.w));
	r.w = v4(0.0f, 0.0f, 0.0f, 1.0f) - r.w;

	return r;
}

FORCE_INLINE m4 transform_inverse(m4 m){
	m4 r;
	// 3x3 transpose
	v4 t0 = v4(MATH_SHUFFLE4(m.x, m.y, 0, 1, 0, 1)); // 1 2 4 5
	v4 t1 = v4(MATH_SHUFFLE4(m.x, m.y, 2, 3, 2, 3)); // 3 0 6 0
	r.x   = v4(MATH_SHUFFLE4(t0, m.z,  0, 2, 0, 3)); // 1 4 7 0
	r.y   = v4(MATH_SHUFFLE4(t0, m.z,  1, 3, 1, 3)); // 2 5 8 0
	r.z   = v4(MATH_SHUFFLE4(t1, m.z,  0, 2, 2, 3)); // 3 6 9 0

	// r.x = [x0, x1, x2, 0]
	// r.y = [y0, y1, y2, 0]

	
	
	
	

	// r.x = [x0, x1, x2, 0]
	// r.y = [y0, y1, y2, 0]
	// r.z = [z0, z1, z2, 0]
	// => [x0^2+y0^2+z0^2,x1^2+y1^2+z1^2, x2^2+y2^2+z2^2, 0]
	// this works since the matrix is transposed! :)
	__m128 size_sqr;
	size_sqr = 											_mm_mul_ps(r.x, r.x);
	size_sqr = _mm_add_ps(size_sqr, _mm_mul_ps(r.y, r.y));
	size_sqr = _mm_add_ps(size_sqr, _mm_mul_ps(r.z, r.z));
	
	// test to avoid divide by zero
	__m128 one = _mm_set1_ps(1.0f);
	// for each element, if size_sqr < epsilon : size_sqr = 1
	constexpr float epsilon = 1.e-8f;
	// size_sqr = [5, 1, 0, 1]
	// mask = [0, 0, 1, 0]
	// r_size_sqr_masked = [1, 1, 0, 1] & [1/5, 1/1, 1/0, 1/1] = [1/5, 1/1, 0, 1/1]
	// one_masked = [0, 0, 1, 0] & [1, 1, 1, 1] = [0, 0, 1, 0]
	// r_size_sqr = [1/5, 1, 0, 1] | [0, 0, 1, 0] = [1/5, 1, 1, 1]
	// If SSE4 is supported the following 4 instructions can be substituted with
	// __m128 r_size_sqr = _mm_blendv_ps(_mm_div_ps(one, size_sqr), one, mask);
	__m128 mask = _mm_cmplt_ps(size_sqr, _mm_set1_ps(epsilon));
	__m128 r_size_sqr_masked = _mm_andnot_ps(mask, _mm_div_ps(one, size_sqr));
	__m128 one_masked       = _mm_and_ps(mask, one);
	__m128 r_size_sqr       = _mm_or_ps(r_size_sqr_masked, one_masked);

	r.x = v4(_mm_mul_ps(r.x, r_size_sqr));
	r.y = v4(_mm_mul_ps(r.y, r_size_sqr));
	r.z = v4(_mm_mul_ps(r.z, r_size_sqr));
	
	// TODO HUR FUNKAR DETTA
	r.w =       r.x*v4(splat_x(m.w)); // [1 1 1 1] * [x x x x] = [1x 1x 1x 1x]
	r.w = r.w + r.y*v4(splat_y(m.w)); // [2 2 2 1] * [y y y y] = [2y 2y 2y 1y]
	r.w = r.w + r.z*v4(splat_z(m.w)); // [3 3 3 1] * [z z z z] = [3z 3z 3z 1z]
																		// [1x+2y+3z, 1x+2y+3z, 1x+2y+3z, 1x+1y+1z]
	r.w = v4(0.0f, 0.0f, 0.0f, 1.0f) - r.w;

	return r;
}

// Multiplies two column major 2x2 matrices.
FORCE_INLINE __m128 mul_2x2(__m128 a, __m128 b){
	// a  = [a0,a1,a2,a3]
	// b  = [b0,b1,b2,b3]
	//
	// a * b = [a0 a2][b0 b2] = [a0*b0 + a2*b1     a0*b2+a2*b3]
	//         [a1 a3][b1 b3]   [a1*b0 + a3*b1     a1*b2+a3*b3]
	//
	// t0 = [a0,a1,a2,a3]*[b0,b0,b3,b3] = [a0*b0, a1*b0, a2*b3, a3*b3]
	// t1 = [a2,a3,a0,a1]*[b1,b1,b2,b2] = [a2*b1, a3*b1, a0*b2, a1*b2]
	// r  = [a0*b0 + a2*b1     a2*b3 + a0*b2]
	//      [a1*b0 + a3*b1     a3*b3 + a1*b2]
	//
	return _mm_add_ps(_mm_mul_ps(a, MATH_SHUFFLE4(b,b, 0,0,3,3)),
										_mm_mul_ps(MATH_SHUFFLE4(a,a, 2,3,0,1), MATH_SHUFFLE4(b,b, 1,1,2,2)));
}

FORCE_INLINE __m128 adj_mul_2x2(__m128 a, __m128 b){
	// a      = [a0, a1, a2, a3]
	// adj(a) = [a3,-a1,-a2, a0]
	// b      = [b0, b1, b2, b3]
	//
	// adj(a) * b = [ a3 -a2][b0 b2] = [ a3*b0 - a2*b1      a3*b2-a2*b3]
	//              [-a1  a0][b1 b3]   [-a1*b0 + a0*b1     -a1*b2+a0*b3]
	//
	// t0 = [ a3, a0, a3, a0]*[b0,b1,b2,b3] = [ a3*b0,  a0*b1,  a3*b2,  a0*b3]
	// t1 = [-a2,-a1,-a2,-a1]*[b1,b0,b3,b2] = [-a2*b1, -a1*b0, -a2*b3, -a1*b2]
	// r  = [ a3*b0 - a2*b1      a3*b2 - a2*b3]
	//      [ a0*b1 - a1*b0      a0*b3 - a1*b2]
	return _mm_sub_ps(_mm_mul_ps(MATH_SHUFFLE4(a,a, 3,0,3,0), b),
		                _mm_mul_ps(MATH_SHUFFLE4(a,a, 2,1,2,1), MATH_SHUFFLE4(b,b, 1,0,3,2)));
}

FORCE_INLINE __m128 mul_adj_2x2(__m128 a, __m128 b){
	// a      = [a0, a1, a2, a3]
	// b      = [b0, b1, b2, b3]
	// adj(b) = [b3,-b1,-b2, b0]
	//
	// a * adj(b) = [a0 a2][ b3 -b2] = [ a0*b3 - a2*b1      a2*b0-a0*b2]
	//              [a1 a3][-b1  b0]   [ a1*b3 - a3*b1      a3*b0-a1*b2]
	//
	// t0 = [a0,a1,a2,a3]*[b3,b3,b0,b0] = [a0*b3, a1*b3, a2*b0, a3*b0]
	// t1 = [a2,a3,a0,a1]*[b1,b1,b2,b2] = [a2*b1, a3*b1, a0*b2, a1*b2]
	// r  = [a0*b3 - a2*b1      a2*b0 - a0*b2]
	//      [a1*b3 - a3*b1      a3*b0 - a1*b2]
	return _mm_sub_ps(_mm_mul_ps(a, MATH_SHUFFLE4(b,b, 3,3,0,0)),
		                _mm_mul_ps(MATH_SHUFFLE4(a,a, 2,3,0,1), MATH_SHUFFLE4(b,b, 1,1,2,2)));
}

FORCE_INLINE m4 inverse(m4 m){
	// use block matrix method
	// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

	// sub matrices
	__m128 A = MATH_SHUFFLE4(m.x, m.y, 0,1,0,1);
	__m128 C = MATH_SHUFFLE4(m.x, m.y, 2,3,2,3);
	__m128 B = MATH_SHUFFLE4(m.z, m.w, 0,1,0,1);
	__m128 D = MATH_SHUFFLE4(m.z, m.w, 2,3,2,3);

	//__m128 detA = _mm_set1_ps(m[0][0] * inM.m[1][1] - inM.m[0][1] * inM.m[1][0]);
	//__m128 detC = _mm_set1_ps(m[0][2] * inM.m[1][3] - inM.m[0][3] * inM.m[1][2]);
	//__m128 detB = _mm_set1_ps(m[2][0] * inM.m[3][1] - inM.m[2][1] * inM.m[3][0]);
	//__m128 detD = _mm_set1_ps(m[2][2] * inM.m[3][3] - inM.m[2][3] * inM.m[3][2]);

#if 1 // for determinant, float version is faster
	// determinant as (|A| |C| |B| |D|)
	__m128 detSub = _mm_sub_ps(
		_mm_mul_ps(MATH_SHUFFLE4(m.x, m.z, 0,2,0,2), MATH_SHUFFLE4(m.y, m.w, 1,3,1,3)),
		_mm_mul_ps(MATH_SHUFFLE4(m.x, m.z, 1,3,1,3), MATH_SHUFFLE4(m.y, m.z, 0,2,0,2))
		);
	__m128 detA = MATH_SHUFFLE4(detSub, detSub, 0,0,0,0);
	__m128 detC = MATH_SHUFFLE4(detSub, detSub, 1,1,1,1);
	__m128 detB = MATH_SHUFFLE4(detSub, detSub, 2,2,2,2);
	__m128 detD = MATH_SHUFFLE4(detSub, detSub, 3,3,3,3);
#endif

	// let iM = 1/|M| * | X  Y |
	//                  | Z  W |

	// D#C
	__m128 D_C = adj_mul_2x2(D, C);
	// A#B
	__m128 A_B = adj_mul_2x2(A, B);
	// X# = |D|A - B(D#C)
	__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), mul_2x2(B, D_C));
	// W# = |A|D - C(A#B)
	__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), mul_2x2(C, A_B));

	// |M| = |A|*|D| + ... (continue later)
	__m128 detM = _mm_mul_ps(detA, detD);

	// Y# = |B|C - D(A#B)#
	__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), mul_adj_2x2(D, A_B));
	// Z# = |C|B - A(D#C)#
	__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), mul_adj_2x2(A, D_C));

	// |M| = |A|*|D| + |B|*|C| ... (continue later)
	detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

	// tr((A#B)(D#C))
	__m128 tr = _mm_mul_ps(A_B, MATH_SHUFFLE4(D_C, D_C, 0,2,1,3));
	tr = _mm_hadd_ps(tr, tr);
	tr = _mm_hadd_ps(tr, tr);
	// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C))
	detM = _mm_sub_ps(detM, tr);

	const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
	// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
	__m128 rDetM = _mm_div_ps(adjSignMask, detM);

	X_ = _mm_mul_ps(X_, rDetM);
	Y_ = _mm_mul_ps(Y_, rDetM);
	Z_ = _mm_mul_ps(Z_, rDetM);
	W_ = _mm_mul_ps(W_, rDetM);

	m4 r;

	// apply adjugate and store, here we combine adjugate shuffle and store shuffle
	r.x = v4(MATH_SHUFFLE4(X_, Z_, 3,1,3,1));
	r.y = v4(MATH_SHUFFLE4(X_, Z_, 2,0,2,0));
	r.z = v4(MATH_SHUFFLE4(Y_, W_, 3,1,3,1));
	r.w = v4(MATH_SHUFFLE4(Y_, W_, 2,0,2,0));

	return r;
}

// globals
WEAK_SYMBOL m4 identity_m4 = m4_from_cols(
		v4(1.0f, 0.0f, 0.0f, 0.0f),
		v4(0.0f, 1.0f, 0.0f, 0.0f),
		v4(0.0f, 0.0f, 1.0f, 0.0f),
		v4(0.0f, 0.0f, 0.0f, 1.0f));





}
