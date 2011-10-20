#ifndef __TCODE_UNICODE_H__
#define __TCODE_UNICODE_H__

#include "common.h"

size_t tcode_char_utf8_normalize(tc_char8* out, const tc_char8* in, size_t* inlen);
size_t tcode_char_utf8_to_utf16(tc_char16* out, const tc_char8* in, size_t* inlen, int strict);
size_t tcode_char_utf8_to_utf32(tc_char32* out, const tc_char8* in, size_t* inlen, int strict);

size_t tcode_char_utf16_normalize(tc_char16* out, const tc_char16* in, size_t* inlen);
size_t tcode_char_utf16_to_utf8(tc_char8* out, const tc_char16* in, size_t* inlen, int strict);
size_t tcode_char_utf16_to_utf32(tc_char32* out, const tc_char16* in, size_t* inlen, int strict);

size_t tcode_char_utf32_normalize(tc_char32* out, const tc_char32* in, size_t* inlen);
size_t tcode_char_utf32_to_utf8(tc_char8* out, const tc_char32* in, size_t* inlen, int strict);
size_t tcode_char_utf32_to_utf16(tc_char16* out, const tc_char32* in, size_t* inlen, int strict);

size_t tcode_utf8_normalize(tc_char8* out, size_t outlen, const tc_char8* in, size_t inlen);
size_t tcode_utf8_to_utf16(tc_char16* out, size_t outlen, const tc_char8* in, size_t inlen, int strict);
size_t tcode_utf8_to_utf32(tc_char32* out, size_t outlen, const tc_char8* in, size_t inlen, int strict);

size_t tcode_utf16_normalize(tc_char16* out, size_t outlen, const tc_char16* in, size_t inlen);
size_t tcode_utf16_to_utf8(tc_char8* out, size_t outlen, const tc_char16* in, size_t inlen, int strict);
size_t tcode_utf16_to_utf32(tc_char32* out, size_t outlen, const tc_char16* in, size_t inlen, int strict);

size_t tcode_utf32_to_utf8(tc_char8* out, size_t outlen, const tc_char32* in, size_t inlen, int strict);
size_t tcode_utf32_to_utf16(tc_char16* out, size_t outlen, const tc_char32* in, size_t inlen, int strict);

#endif /* __TCODE_UNICODE_H__ */
