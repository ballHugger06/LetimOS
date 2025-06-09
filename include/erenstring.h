#include <typedefs.h>
#include <erenmath.h>

#ifndef _ERENSTRING_H_
#define _ERENSTRING_H_
s32 strsame(char* first, char* second) {
    if (first[0] != second[0]) {
        return 0;
    }
    u32 i = 1;
    while ((first[i - 1] != 0)) {
        if (first[i] != second[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

//turns i into a string and puts it in buffer
//if bufsize is less than what is required, returns 0
//returns 1 upon success
s32 int2str(s64 n, char* buffer, u64 bufsize) {
	s64 j = 1;
	for (s64 i = 1 - (2 * (n < 0)); n / i > 9; i *= 10) {
		j++;
	}

	if (bufsize < (j + 1 + (n < 0))) return 0;

	if (n < 0) {
		buffer[0] = '-';
		buffer++;
		n *= -1;
	}

	u64 i;
	for (i = 0; i < j; i++) {
		char r = (u8)(n / power(10, j - 1 - i));
		n -= r * power(10, j - 1 - i);
		buffer[i] = r + '0';
	}
	buffer[i] = 0;
	return 1;
}

#endif
