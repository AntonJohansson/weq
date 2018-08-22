#pragma once

#include "vector.hpp"

namespace simd{

FORCE_INLINE v3 abs(v3 a)      {return v3(_mm_andnot_ps(v3_mask_sign, a));}
FORCE_INLINE v3 min(v3 a, v3 b){return v3(_mm_min_ps(a, b));}
FORCE_INLINE v3 max(v3 a, v3 b){return v3(_mm_max_ps(a, b));}
FORCE_INLINE v4 abs(v4 a)      {return v4(_mm_andnot_ps(v4_mask_sign, a));}
FORCE_INLINE v4 min(v4 a, v3 b){return v4(_mm_min_ps(a, b));}
FORCE_INLINE v4 max(v4 a, v3 b){return v4(_mm_max_ps(a, b));}

FORCE_INLINE vfloat abs(vfloat a)          {return vfloat(_mm_andnot_ps(v4_mask_sign, a));}
FORCE_INLINE vfloat min(vfloat a, vfloat b){return vfloat(_mm_min_ps(a, b));}
FORCE_INLINE vfloat max(vfloat a, vfloat b){return vfloat(_mm_max_ps(a, b));}

FORCE_INLINE v3 select(v3 a, v3 b, v3 mask){return v3(_mm_xor_ps(a, _mm_and_ps(mask , _mm_xor_ps(b, a))));}
FORCE_INLINE v4 select(v4 a, v4 b, v4 mask){return v4(_mm_xor_ps(a, _mm_and_ps(mask , _mm_xor_ps(b, a))));}

FORCE_INLINE v3 lerp(v3 a, v3 b, vfloat t){return a + (b - a) * t;}
FORCE_INLINE v4 lerp(v4 a, v4 b, vfloat t){return a + (b - a) * t;}

FORCE_INLINE vfloat hmin(v3 a){
	v3 t0 = v3(MATH_SHUFFLE3(a, a, 1, 0, 2));
	a = min(a, t0);
	v3 t1 = v3(MATH_SHUFFLE3(a, a, 2, 0, 1));
	return vfloat(min(a, t1));
}

FORCE_INLINE vfloat hmin(v4 a){
	v3 t0 = v3(MATH_SHUFFLE4(a, a, 1, 0, 2, 3));
	a = min(a, t0);
	v3 t1 = v3(MATH_SHUFFLE4(a, a, 2, 0, 1, 3));
	a = min(a, t1);
	v3 t2 = v3(MATH_SHUFFLE4(a, a, 3, 0, 2, 1));
	return vfloat(min(a, t2));
}

FORCE_INLINE vfloat hmax(v3 a){
	v3 t0 = v3(MATH_SHUFFLE3(a, a, 1, 0, 2));
	a = max(a, t0);
	v3 t1 = v3(MATH_SHUFFLE3(a, a, 2, 0, 1));
	return vfloat(max(a, t1));
}

FORCE_INLINE vfloat hmax(v4 a){
	v3 t0 = v3(MATH_SHUFFLE4(a, a, 1, 0, 2, 3));
	a = max(a, t0);
	v3 t1 = v3(MATH_SHUFFLE4(a, a, 2, 0, 1, 3));
	a = max(a, t1);
	v3 t2 = v3(MATH_SHUFFLE4(a, a, 3, 0, 2, 1));
	return vfloat(max(a, t2));
}

FORCE_INLINE v3 clamp(v3 a, v3 vmin, v3 vmax){
	v3 t0 = max(vmin, a);
	return min(t0, vmax);
}

FORCE_INLINE v4 clamp(v4 a, v4 vmin, v4 vmax){
	v4 t0 = max(vmin, a);
	return min(t0, vmax);
}

}
