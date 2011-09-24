#ifndef __TCODE_BASE64_H__
#define __TCODE_BASE64_H__

#include <stddef.h>

size_t tcode_base64enclen(size_t inlen, int pad);
size_t tcode_base64declen(size_t inlen);

size_t tcode_base64encode(char* out, size_t outlen, const char* in, size_t inlen, char ch62, char ch63, int pad);
size_t tcode_base64decode(char* out, size_t outlen, const char* in, size_t inlen, int strict, char ch62, char ch63, int pad);

#endif /* __TCODE_BASE64_H__ */
