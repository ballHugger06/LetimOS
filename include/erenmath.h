#include <typedefs.h>

#ifndef _ERENMATH_H_
#define _ERENMATH_H_

u64 power(u64 base, u64 power) {
	if (power == 0) {
		return 1;
	}

	u64 result = 1;
	for (u64 i = 0; i != power; i++) {
		result *= base;
	}
	return result;
}

#endif
