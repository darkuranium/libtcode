#ifndef __TCODE_INTERNAL_H__
#define __TCODE_INTERNAL_H__

#include <tcode/common.h>

#include <stddef.h>

#define MIN(x,y) (((x) < (y)) ? (x) : (y))

size_t _tcode_append8(size_t pos, tc_char8* out, size_t outlen, const tc_char8* in, size_t inlen);
size_t _tcode_append16(size_t pos, tc_char16* out, size_t outlen, const tc_char16* in, size_t inlen);
size_t _tcode_append32(size_t pos, tc_char32* out, size_t outlen, const tc_char32* in, size_t inlen);

size_t _tcode_append(size_t pos, char* out, size_t outlen, const char* in, size_t inlen);
int _tcode_startswith(size_t pos, const char* in, size_t inlen, const char* what, size_t whatlen);
/* integer division, rounded up */
size_t _tcode_ceildiv(size_t p, size_t q);

static const char TcodeBase64EncodeTable[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', /*'+', '/',*/
        0 /* < this allows us to use the table with strchr for testing validity! */
    };
/* todo: get this into a format that ANSI C compilers will accept */
static const char TcodeBase64DecodeTable[256] = {
        ['A'] =  0, ['B'] =  1, ['C'] =  2, ['D'] =  3,
        ['E'] =  4, ['F'] =  5, ['G'] =  6, ['H'] =  7,
        ['I'] =  8, ['J'] =  9, ['K'] = 10, ['L'] = 11,
        ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15,
        ['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19,
        ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
        ['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27,
        ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31,
        ['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35,
        ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39,
        ['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43,
        ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47,
        ['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51,
        ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55,
        ['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59,
        ['8'] = 60, ['9'] = 61, /*['+'] = 62, ['/'] = 63,*/
    };

#endif /* __TCODE_INTERNAL_H__ */
