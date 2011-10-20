#include <tcode/internal.h>
#include <tcode/url.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

size_t tcode_urlencode(char* out, size_t outlen, const char* in, size_t inlen, int useplus, int enctilde)
{
    size_t len = 0;

    char buf[4];

    size_t i;
    for(i = 0; i < inlen; i++)
    {
        if(isalnum(in[i]) || strchr("-_.", in[i]) || (in[i] == '~' && !enctilde))
            len += _tcode_append(len, out, outlen, &in[i], 1);
        else if(in[i] == ' ' && useplus)
            len += _tcode_append(len, out, outlen, "+", 1);
        else
        {
            sprintf(buf, "%%%.2X", in[i]);
            len += _tcode_append(len, out, outlen, buf, 3);
        }
    }
    out[MIN(outlen-1,len)] = 0;

    return len;
}
size_t tcode_urldecode(char* out, size_t outlen, const char* in, size_t inlen, int strict)
{
    size_t len = 0;

    char buf[3] = {0};

    size_t i;
    for(i = 0; i < inlen; i++)
    {
        if(in[i] == '+')
            len += _tcode_append(len, out, outlen, " ", 1);
        else if(in[i] == '%')
        {
            if(!isxdigit(in[i+1]) || !isxdigit(in[i+2]))
            {
                if(strict)
                    goto error;
                /* not strict */
                len += _tcode_append(len, out, outlen, &in[i], 1);
                continue;
            }
            memcpy(buf, &in[i+1], 2);
            buf[0] = strtoul(buf, NULL, 16);
            len += _tcode_append(len, out, outlen, &buf[0], 1);
            i += 2;
        }
        else
            len += _tcode_append(len, out, outlen, &in[i], 1);
    }
    out[MIN(outlen-1,len)] = 0;

    return len;
error:
    out[MIN(outlen-1,len)] = 0;
    errno = EILSEQ;
    return 0;
}
