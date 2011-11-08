#include <tcode/internal.h>

#include <string.h>

size_t _tcode_append8(size_t pos, tc_char8* out, size_t outlen, const tc_char8* in, size_t inlen)
{
    if(pos + 1 >= outlen)
        return inlen;

    memcpy(out + pos, in, MIN(outlen - pos - 1, inlen) * sizeof(tc_char8));
    return inlen;
}
size_t _tcode_append16(size_t pos, tc_char16* out, size_t outlen, const tc_char16* in, size_t inlen)
{
    if(pos + 1 >= outlen)
        return inlen;

    memcpy(out + pos, in, MIN(outlen - pos - 1, inlen) * sizeof(tc_char16));
    return inlen;
}
size_t _tcode_append32(size_t pos, tc_char32* out, size_t outlen, const tc_char32* in, size_t inlen)
{
    if(pos + 1 >= outlen)
        return inlen;

    memcpy(out + pos, in, MIN(outlen - pos - 1, inlen) * sizeof(tc_char32));
    return inlen;
}

size_t _tcode_append(size_t pos, char* out, size_t outlen, const char* in, size_t inlen)
{
    return _tcode_append8(pos, out, outlen, in, inlen);
}
int _tcode_startswith(size_t pos, const char* in, size_t inlen, const char* what, size_t whatlen)
{
    /* rest of string too small, so it can't possibly start with that. */
    if(inlen - pos < whatlen)
        return 0;

    size_t i;
    for(i = 0; i < whatlen; i++)
        if(in[pos + i] != what[i])
            return 0;
    return 1;
}
/* integer division, rounded up */
size_t _tcode_ceildiv(size_t p, size_t q)
{
    return (p / q) + ((p % q) ? 1 : 0);
}
