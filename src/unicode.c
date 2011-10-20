#include <tcode/internal.h>
#include <tcode/unicode.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h> 

typedef size_t (tc_normalize_func)(void* out, const void* in, size_t* inlen);
typedef size_t (tc_convert_func)(void* out, const void* in, size_t* inlen, int strict);
typedef size_t (tc_append_func)(size_t num, void* out, size_t outlen, const void* in, size_t inlen);
static size_t _tcode_normalize(size_t size,
                               void* out, size_t outlen,
                               const void* in, size_t inlen,
                               tc_normalize_func* normalize, tc_append_func* append)
{
    size_t ilen;
    size_t len;
	char buf[4];
	size_t num = 0;
	
	size_t i;
	for(i = 0; i < inlen; i++)
	{
        ilen = inlen - i;
        len = normalize(buf, ((const char*)in) + i * size, &ilen);
        if(!len)
            goto error;
        i += (inlen - i) - ilen - 1;
        
		num += append(num, out, outlen, buf, len);
	}
    memset(((char*)out) + MIN(num, outlen-1) * size, 0, size); /* 0-terminate */
	return num;
error:
    memset(((char*)out) + MIN(num, outlen-1) * size, 0, size); /* 0-terminate */
    return 0;
}
static size_t _tcode_convert(size_t outsize, size_t insize,
                             void* out, size_t outlen,
                             const void* in, size_t inlen,
                             int strict,
                             tc_convert_func* convert, tc_append_func* append)
{
    size_t ilen;
    size_t len;
	char buf[4];
	size_t num = 0;
	
	size_t i;
	for(i = 0; i < inlen; i++)
	{
        ilen = inlen - i;
        len = convert(buf, ((const char*)in) + i * insize, &ilen, strict);
        if(!len)
            goto error;
        i += (inlen - i) - ilen - 1;
        
		num += append(num, out, outlen, buf, len);
	}
    memset(((char*)out) + MIN(num, outlen-1) * outsize, 0, outsize); /* 0-terminate */
	return num;
error:
    memset(((char*)out) + MIN(num, outlen-1) * outsize, 0, outsize); /* 0-terminate */
    return 0;
}

size_t tcode_char_utf8_normalize(tc_char8* out, const tc_char8* in, size_t* inlen)
{
    size_t ilen = 1;
    tc_char32 buf;
    size_t len = tcode_char_utf8_to_utf32(&buf, in, inlen, 0);
    if(!len)
        return 0;
    return tcode_char_utf32_to_utf8(out, &buf, &ilen, 0);
}
size_t tcode_char_utf8_to_utf16(tc_char16* out, const tc_char8* in, size_t* inlen, int strict)
{
    size_t ilen = 1;
    tc_char32 buf;
    size_t len = tcode_char_utf8_to_utf32(&buf, in, inlen, strict);
    if(!len)
        return 0;
    return tcode_char_utf32_to_utf16(out, &buf, &ilen, strict);
}
size_t tcode_char_utf8_to_utf32(tc_char32* out, const tc_char8* in, size_t* inlen, int strict)
{
    (*inlen)--;
    
    const tc_uchar8* uin = (const tc_uchar8*)in;
    
    if(strict && (uin[0] == 0xFF || uin[0] == 0xFE))
    {
        tcode_error_raisef("Invalid value (0x%.2X)", uin[0]);
        return 0;
    }
    
    if(uin[0] & 0x80) /* bit 1 */
    {
        if(uin[0] & 0x40) /* bit 2 */
        {
            if(uin[0] & 0x20) /* bit 3 */
            {
                if(uin[0] & 0x10) /* bit 4 */
                {
                    if(uin[0] & 0x08) /* bit 5 */
                    {
                        tcode_error_raisef("Too long sequence (0x%.2X)", uin[0]);
                        return 0;
                    }
                    else /* +3 bytes */
                    {
                        if(3 > *inlen)
                        {
                            tcode_error_raisef("Unterminated sequence 0x%.2X------", uin[0]);
                            return 0;
                        }
                        out[0] = (uin[0] & 0x07) << 18;
                        (*inlen)--;
                        out[0] |= (uin[1] & 0x3F) << 12;
                        (*inlen)--;
                        out[0] |= (uin[2] & 0x3F) << 6;
                        (*inlen)--;
                        out[0] |= (uin[3] & 0x3F);
                        
                        if(strict && out[0] < 0x010000)
                        {
                            tcode_error_raisef("Un-normalized input (0x%.6X)", uin[0]);
                            return 0;
                        }
                    }
                }
                else /* +2 bytes */
                {
                    if(2 > *inlen)
                    {
                        tcode_error_raisef("Unterminated sequence 0x%.2X----", uin[0]);
                        return 0;
                    }
                    out[0] = (uin[0] & 0x0F) << 12;
                    (*inlen)--;
                    out[0] |= (uin[1] & 0x3F) << 6;
                    (*inlen)--;
                    out[0] |= (uin[2] & 0x3F);
                    
                    if(strict && out[0] < 0x000800)
                    {
                        tcode_error_raisef("Un-normalized input (0x%.4X)", uin[0]);
                        return 0;
                    }
                }
            }
            else /* +1 byte */
            {
                if(1 > *inlen)
                {
                    tcode_error_raisef("Unterminated sequence 0x%.2X--", uin[0]);
                    return 0;
                }
                out[0] = (uin[0] & 0x1F) << 6;
                (*inlen)--;
                out[0] |= uin[1] & 0x3F;
                
                if(strict && out[0] < 0x000080)
                {
                    tcode_error_raisef("Un-normalized input (0x%.2X)", uin[0]);
                    return 0;
                }
            }
        }
        else
        {
            tcode_error_raisef("Found sequence continuation character without start (0x%.2X)", uin[0]);
            return 0;
        }
    }
    else /* +0 bytes (ASCII) */
        out[0] = uin[0] & 0x7F;
    return 1;
}

size_t tcode_char_utf16_normalize(tc_char16* out, const tc_char16* in, size_t *inlen)
{
    size_t ilen = 1;
    tc_char32 buf;
    size_t len = tcode_char_utf16_to_utf32(&buf, in, inlen, 0);
    if(!len)
        return 0;
    return tcode_char_utf32_to_utf16(out, &buf, &ilen, 0);
}
size_t tcode_char_utf16_to_utf8(tc_char8* out, const tc_char16* in, size_t* inlen, int strict)
{
    size_t ilen = 1;
    tc_char32 buf;
    size_t len = tcode_char_utf16_to_utf32(&buf, in, inlen, strict);
    if(!len)
        return 0;
    return tcode_char_utf32_to_utf8(out, &buf, &ilen, strict);
}
size_t tcode_char_utf16_to_utf32(tc_char32* out, const tc_char16* in, size_t* inlen, int strict)
{
    (*inlen)--;
    
    if(0xD800 <= in[0] && in[0] <= 0xDBFF)
    {
        if(1 > *inlen)
        {
            tcode_error_raisef("Unterminated sequence 0x.4X----", in[0]);
            return 0;
        }
        out[0] = (in[0] & 0x3FF) << 10;
        (*inlen)--;
        out[0] |= in[1] & 0x3FF;
        out[0] += 0x10000;
        return 1;
    }
    else if(in[0] < 0xD800 || 0xDFFF < in[0])
    {
        out[0] = in[0];
        return 1;
    }
    
    /* error */
    tcode_error_raisef("Errorneous sequence (0x%.4X)", in[0]);
    return 0;
}

size_t tcode_char_utf32_to_utf8(tc_char8* out, const tc_char32* in, size_t* inlen, int strict)
{
    (*inlen)--;
    
    if(in[0] <= 0x7F) /* 1 octet */
    {
        out[0] = in[0];
        return 1;
    }
    else if(in[0] <= 0x07FF) /* 2 octets */
    {
        out[0] = 0xC0 | (in[0] >> 6);
        out[1] = 0x80 | (in[0] & 0x3F);
        return 2;
    }
    else if(in[0] <= 0xFFFF) /* 3 octets */
    {
        out[0] = 0xE0 | (in[0] >> 12);
        out[1] = 0x80 | ((in[0] >> 6) & 0x3F);
        out[2] = 0x80 | (in[0] & 0x3F);
        return 3;
    }
    else if(in[0] <= 0x10FFFF) /* 4 octets */
    {
        out[0] = 0xF0 | (in[0] >> 18);
        out[1] = 0x80 | ((in[0] >> 12) & 0x3F);
        out[2] = 0x80 | ((in[0] >> 6) & 0x3F);
        out[3] = 0x80 | (in[0] & 0x3F);
        return 4;
    }
    
    /* error */
    tcode_error_raisef("Cannot encode value %.8X into UTF-8 (it is larger than 0x10FFFF)", in[0]);
    return 0;
}
size_t tcode_char_utf32_to_utf16(tc_char16* out, const tc_char32* in, size_t* inlen, int strict)
{
    (*inlen)--;
    
    if(strict && 0xD800 <= in[0] && in[0] <= 0xDFFF) /* reserved for UTF-16 */
    {
        tcode_error_raisef("Cannot encode value %.4X into UTF-16 (value reserved for the UTF-16 encoding itself)", in[0]);
        return 0;
    }
    
    if(in[0] <= 0xFFFF) /* 2 octets */
    {
        out[0] = in[0];
        return 1;
    }
    else if(in[0] <= 0x10FFFF) /* 4 octets */
    {
        out[0] = 0xD800 | ((in[0] - 0x10000) >> 10);
        out[1] = 0xDC00 | ((in[0] - 0x10000) & 0x3FF);
        return 2;
    }
    
    /* error */
    tcode_error_raisef("Cannot encode value %.8X into UTF-16 (it is larger than 0x10FFFF)", in[0]);
    return 0;
}

size_t tcode_utf8_normalize(tc_char8* out, size_t outlen, const tc_char8* in, size_t inlen)
{
    return _tcode_normalize(sizeof(*out),
                            out, outlen,
                            in, inlen,
                            (tc_normalize_func*)tcode_char_utf8_normalize, (tc_append_func*)_tcode_append8);
}
size_t tcode_utf8_to_utf16(tc_char16* out, size_t outlen, const tc_char8* in, size_t inlen, int strict)
{
    return _tcode_convert(sizeof(*out), sizeof(*in),
                          out, outlen,
                          in, inlen,
                          strict,
                          (tc_convert_func*)tcode_char_utf8_to_utf16, (tc_append_func*)_tcode_append16);
}
size_t tcode_utf8_to_utf32(tc_char32* out, size_t outlen, const tc_char8* in, size_t inlen, int strict)
{
    return _tcode_convert(sizeof(*out), sizeof(*in),
                          out, outlen,
                          in, inlen,
                          strict,
                          (tc_convert_func*)tcode_char_utf8_to_utf32, (tc_append_func*)_tcode_append32);
}

size_t tcode_utf16_normalize(tc_char16* out, size_t outlen, const tc_char16* in, size_t inlen)
{
    return _tcode_normalize(sizeof(*out),
                            out, outlen,
                            in, inlen,
                            (tc_normalize_func*)tcode_char_utf16_normalize, (tc_append_func*)_tcode_append16);
}
size_t tcode_utf16_to_utf8(tc_char8* out, size_t outlen, const tc_char16* in, size_t inlen, int strict)
{
    return _tcode_convert(sizeof(*out), sizeof(*in),
                          out, outlen,
                          in, inlen,
                          strict,
                          (tc_convert_func*)tcode_char_utf16_to_utf8, (tc_append_func*)_tcode_append8);
}
size_t tcode_utf16_to_utf32(tc_char32* out, size_t outlen, const tc_char16* in, size_t inlen, int strict)
{
    return _tcode_convert(sizeof(*out), sizeof(*in),
                          out, outlen,
                          in, inlen,
                          strict,
                          (tc_convert_func*)tcode_char_utf16_to_utf32, (tc_append_func*)_tcode_append32);
}

size_t tcode_utf32_to_utf8(tc_char8* out, size_t outlen, const tc_char32* in, size_t inlen, int strict)
{
    return _tcode_convert(sizeof(*out), sizeof(*in),
                          out, outlen,
                          in, inlen,
                          strict,
                          (tc_convert_func*)tcode_char_utf32_to_utf8, (tc_append_func*)_tcode_append8);
}
size_t tcode_utf32_to_utf16(tc_char16* out, size_t outlen, const tc_char32* in, size_t inlen, int strict)
{
    return _tcode_convert(sizeof(*out), sizeof(*in),
                          out, outlen,
                          in, inlen,
                          strict,
                          (tc_convert_func*)tcode_char_utf32_to_utf16, (tc_append_func*)_tcode_append16);
}
