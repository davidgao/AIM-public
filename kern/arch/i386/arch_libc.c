#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "sys/types.h"
#include "asm.h"

void *memset(void *dst, int c, size_t n) {
    stosb(dst, c, (uint32_t)n);
    return dst;
}

int strcmp(const char *a, const char *b) {
    while(*a == *b && *a!='\0') {
        a++; b++;
    }
    if(*a == *b) return 0;
    else
        return *a - *b;
}

void *memmove(void *dst00, const void *src00, size_t length) {
	char *src0 = (char *)src00, *dst0 = (char *)dst00;
	if(src0 > dst0 && src0 < dst0 + length) {
		// sequential
		for(int i=0; i<length; ++i) {
			*dst0++ = *src0++;
		}
	}
	else {
		// reverse
		src0 += length - 1;
		dst0 += length - 1;
		for(int i=0; i<length; ++i) {
			*dst0-- = *src0--;
		}
	}
	return dst00;
}