#pragma once

#ifdef _WIN32
	#define FORCE_INLINE __forceinline
	#define WEAK_SYMBOL extern const __declspec(selectany)
#else
	#define FORCE_INLINE __attribute__((always_inline)) inline
	#define WEAK_SYMBOL extern const __attribute__((weak))
#endif
