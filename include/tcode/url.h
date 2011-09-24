#ifndef __TCODE_URL_H__
#define __TCODE_URL_H__

#include <stddef.h>

size_t tcode_urlencode(char* out, size_t outlen, const char* in, size_t inlen, int useplus, int enctilde);
size_t tcode_urldecode(char* out, size_t outlen, const char* in, size_t inlen, int strict);

#endif /* __TCODE_URL_H__ */
