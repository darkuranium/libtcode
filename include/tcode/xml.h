#ifndef __TCODE_XML_H__
#define __TCODE_XML_H__

#include <stddef.h>

size_t tcode_xmlencode(char* out, size_t outlen, const char* in, size_t inlen, int usequot, int useapos);
size_t tcode_xmldecode(char* out, size_t outlen, const char* in, size_t inlen, int strict);

#endif /* __TCODE_XML_H__ */
