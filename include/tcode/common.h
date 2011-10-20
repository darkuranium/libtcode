#ifndef __TCODE_COMMON_H__
#define __TCODE_COMMON_H__

#include <stddef.h>
#include <stdarg.h>

typedef unsigned char  tc_uchar8;

typedef char           tc_char8;
typedef unsigned short tc_char16;
typedef unsigned int   tc_char32;

void tcode_error_raisefv(const char* format, va_list args);
void tcode_error_raisef(const char* format, ...);
void tcode_error_raise(const char* msg);

char* tcode_error_getmsg(void);
void tcode_error_print(const char* prefix);

#endif /* __TCODE_COMMON_H__ */
