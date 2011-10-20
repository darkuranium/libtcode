#include <tcode/internal.h>
#include <tcode/unicode.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h> 

size_t tcode_char_utf8_normalize(tc_char8* out, const tc_char8* in, size_t* inlen);
size_t tcode_char_utf8_to_utf16(tc_char16* out, const tc_char8* in, size_t* inlen, int strict);
size_t tcode_char_utf8_to_utf32(tc_char32* out, const tc_char8* in, size_t* inlen, int strict);

size_t tcode_char_utf16_normalize(tc_char16* out, const tc_char16* in, size_t *inlen);
size_t tcode_char_utf16_to_utf8(tc_char8* out, const tc_char16* in, size_t* inlen, int strict);
size_t tcode_char_utf16_to_utf32(tc_char32* out, const tc_char16* in, size_t* inlen, int strict);

size_t tcode_char_utf32_normalize(tc_char32* out, const tc_char32* in, size_t* inlen);
size_t tcode_char_utf32_to_utf8(tc_char8* out, const tc_char32* in, size_t* inlen, int strict);
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

size_t tcode_utf8_normalize(tc_char8* out, size_t outlen, const tc_char8* in, size_t inlen);
size_t tcode_utf8_to_utf16(tc_char16* out, size_t outlen, const tc_char8* in, size_t inlen, int strict);
size_t tcode_utf8_to_utf32(tc_char32* out, size_t outlen, const tc_char8* in, size_t inlen, int strict)
{
	const tc_uchar8* uin = (const tc_uchar8*)in;
	
	tc_char32 buf;
	size_t num = 0;
	
	size_t i;
	for(i = 0; i < inlen; i++)
	{
		if(strict && (uin[i] == 0xFF || uin[i] == 0xFE))
		{
            tcode_error_raisef("Invalid value at position %lu (0x%.2X)", (unsigned long int)i, uin[i]);
			return 0;
		}
		
		if(uin[i] & 0x80) /* bit 1 */
		{
			if(uin[i] & 0x40) /* bit 2 */
			{
				if(uin[i] & 0x20) /* bit 3 */
				{
					if(uin[i] & 0x10) /* bit 4 */
					{
						if(uin[i] & 0x08) /* bit 5 */
						{
                            tcode_error_raisef("Too long sequence at position %lu (0x%.2X)", (unsigned long int)i, uin[i]);
							return 0;
						}
						else /* +3 vrednosti */
						{
							if(i + 3 >= inlen)
							{
                                tcode_error_raisef("Unterminated sequence 0x%.2X------ at position %lu", uin[i], (unsigned long int)i);
								return 0;
							}
							buf = (uin[i] & 0x07) << 18;
							i++;
							buf |= (uin[i] & 0x3F) << 12;
							i++;
							buf |= (uin[i] & 0x3F) << 6;
							i++;
							buf |= (uin[i] & 0x3F);
							
							if(strict && buf < 0x010000)
                            {
                                tcode_error_raisef("Un-normalized input at position %lu (0x%.6X)", (unsigned long int)i, uin[i]);
                                return 0;
                            }
						}
					}
					else /* +2 vrednosti */
					{
						if(i + 2 >= inlen)
						{
							tcode_error_raisef("Unterminated sequence 0x%.2X---- at position %lu", uin[i], (unsigned long int)i);
							return 0;
						}
						buf = (uin[i] & 0x0F) << 12;
						i++;
						buf |= (uin[i] & 0x3F) << 6;
						i++;
						buf |= (uin[i] & 0x3F);
						
						if(strict && buf < 0x000800)
                        {
                            tcode_error_raisef("Un-normalized input at position %lu (0x%.4X)", (unsigned long int)i, uin[i]);
                            return 0;
                        }
					}
				}
				else /* +1 vrednost */
				{
					if(i + 1 >= inlen)
					{
						tcode_error_raisef("Unterminated sequence 0x%.2X-- at position %lu", uin[i], (unsigned long int)i);
						return 0;
					}
					buf = (uin[i] & 0x1F) << 6;
					i++;
					buf |= uin[i] & 0x3F;
					
					if(strict && buf < 0x000080)
                    {
                        tcode_error_raisef("Un-normalized input at position %lu (0x%.2X)", (unsigned long int)i, uin[i]);
                        return 0;
                    }
				}
			}
			else
			{
                tcode_error_raisef("Found sequence continuation character without start at position %lu (0x%.2X)", (unsigned long int)i, uin[i]);
				return 0;
			}
		}
		else /* +0 bytes (ASCII) */
			buf = uin[i] & 0x7F;
		num += _tcode_append32(num, out, outlen, &buf, 1);
	}
	out[MIN(num, outlen-1)] = 0; /* 0-terminate */
	return num;
}

size_t tcode_utf16_normalize(tc_char16* out, size_t outlen, const tc_char16* in, size_t inlen);
size_t tcode_utf16_to_utf8(tc_char8* out, size_t outlen, const tc_char16* in, size_t inlen, int strict);
size_t tcode_utf16_to_utf32(tc_char32* out, size_t outlen, const tc_char16* in, size_t inlen, int strict)
{
    tc_char32 buf;
    size_t num = 0;
    
    size_t i;
    for(i = 0; i < inlen; i++)
    {
        if(0xD800 <= in[i] && in[i] <= 0xDBFF)
        {
            if(i + 1 >= inlen)
            {
                tcode_error_raisef("Unterminated sequence 0x.4X---- at position %u", in[i], (unsigned long int)i);
                return 0;
            }
            buf = (in[i] & 0x3FF) << 10;
            i++;
            buf |= in[i] & 0x3FF;
            buf += 0x10000;
        }
        else if(0xDBFF <= in[i] && in[i] <= 0xDFFF)
        {
            tcode_error_raisef("Errorneous sequence at position %lu (0x%.4X)", (unsigned long int)i, in[i]);
            return 0;
        }
    }
    return num;
}

size_t tcode_utf32_to_utf8(tc_char8* out, size_t outlen, const tc_char32* in, size_t inlen, int strict)
{
    size_t len;
	tc_uchar8 buf[4];
	size_t num = 0;
	
	size_t i;
	for(i = 0; i < inlen; i++)
	{
		if(in[i] <= 0x7F) /* 1 octet */
		{
			buf[0] = in[i];
			len = 1;
		}
		else if(in[i] <= 0x07FF) /* 2 octets */
		{
			buf[0] = 0xC0 | (in[i] >> 6);
			buf[1] = 0x80 | (in[i] & 0x3F);
			len = 2;
		}
		else if(in[i] <= 0xFFFF) /* 3 octets */
		{
			buf[0] = 0xE0 | (in[i] >> 12);
			buf[1] = 0x80 | ((in[i] >> 6) & 0x3F);
			buf[2] = 0x80 | (in[i] & 0x3F);
			len = 3;
		}
		else if(in[i] <= 0x10FFFF) /* 4 octets */
		{
			buf[0] = 0xF0 | (in[i] >> 18);
			buf[1] = 0x80 | ((in[i] >> 12) & 0x3F);
			buf[2] = 0x80 | ((in[i] >> 6) & 0x3F);
			buf[3] = 0x80 | (in[i] & 0x3F);
			len = 4;
		}
		else /* error */
		{
            tcode_error_raisef("Cannot encode value %.8X into UTF-8 (it is larger than 0x10FFFF)", in[i]);
			return 0;
		}
		num += _tcode_append8(num, out, outlen, (tc_char8*)buf, len);
	}
	out[MIN(num, outlen-1)] = 0; /* 0-terminate */
	return num;
}
size_t tcode_utf32_to_utf16(tc_char16* out, size_t outlen, const tc_char32* in, size_t inlen, int strict)
{
    size_t len;
    tc_char16 buf[2];
    size_t num = 0;
    
    size_t i;
    for(i = 0; i < inlen; i++)
    {
        if(strict && 0xD800 <= in[i] && in[i] <= 0xDFFF) /* reserved for UTF-16 */
        {
            tcode_error_raisef("Cannot encode value %.4X into UTF-16 (value reserved for the UTF-16 encoding itself)", in[i]);
            return 0;
        }
        
        if(in[i] <= 0xFFFF) /* 2 octets */
        {
            buf[0] = in[i];
            len = 1;
        }
        else if(in[i] <= 0x10FFFF) /* 4 octets */
        {
            buf[0] = 0xD800 | ((in[i] - 0x10000) >> 10);
            buf[1] = 0xDC00 | ((in[i] - 0x10000) & 0x3FF);
            len = 2;
        }
        else /* error */
        {
            tcode_error_raisef("Cannot encode value %.8X into UTF-16 (it is larger than 0x10FFFF)", in[i]);
            return 0;
        }
        num += _tcode_append16(num, out, outlen, buf, len);
    }
    out[MIN(num, outlen-1)] = 0; /* 0-terminate */
    return num;
}
