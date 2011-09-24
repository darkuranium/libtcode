#include <tcode/internal.h>
#include <tcode/xml.h>

#include <string.h>
#include <errno.h>

size_t tcode_xmlencode(char* out, size_t outlen, const char* in, size_t inlen, int usequot, int useapos)
{
    size_t len = 0;

    size_t i;
    for(i = 0; i < inlen; i++)
    {
        if(in[i] == '"' && usequot)
            len += _tcode_append(len, out, outlen, "&quot;", 6);
        else if(in[i] == '\'' && useapos)
            len += _tcode_append(len, out, outlen, "&apos;", 6);
        else if(in[i] == '<')
            len += _tcode_append(len, out, outlen, "&lt;", 4);
        else if(in[i] == '>')
            len += _tcode_append(len, out, outlen, "&gt;", 4);
        else if(in[i] == '&')
            len += _tcode_append(len, out, outlen, "&amp;", 5);
        else
            len += _tcode_append(len, out, outlen, &in[i], 1);
    }
    out[MIN(outlen-1,len)] = 0;

    return len;
}
size_t tcode_xmldecode(char* out, size_t outlen, const char* in, size_t inlen, int strict)
{
    size_t len = 0;

    size_t i;
    for(i = 0; i < inlen; i++)
    {
        if(_tcode_startswith(i, in, inlen, "&quot;", 6))
        {
            len += _tcode_append(len, out, outlen, "\"", 1);
            i += 5;
        }
        else if(_tcode_startswith(i, in, inlen, "&apos;", 6))
        {
            len += _tcode_append(len, out, outlen, "'", 1);
            i += 5;
        }
        else if(_tcode_startswith(i, in, inlen, "&lt;", 4))
        {
            len += _tcode_append(len, out, outlen, "<", 1);
            i += 3;
        }
        else if(_tcode_startswith(i, in, inlen, "&gt;", 4))
        {
            len += _tcode_append(len, out, outlen, ">", 1);
            i += 3;
        }
        else if(_tcode_startswith(i, in, inlen, "&amp;", 5))
        {
            len += _tcode_append(len, out, outlen, "&", 1);
            i += 4;
        }
        else if(strict && strchr("<>&", in[i]))
            goto error;
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
