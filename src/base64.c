#include <tcode/internal.h>
#include <tcode/base64.h>

#include <string.h>
#include <errno.h>

size_t tcode_base64enclen(size_t inlen, int pad)
{
    size_t len = _tcode_ceildiv(inlen * 4, 3);
    return len + (pad ? (len % 2) : 0);
}
size_t tcode_base64declen(size_t inlen)
{
    return _tcode_ceildiv(inlen * 3, 4);
}

size_t tcode_base64encode(char* out, size_t outlen, const char* in, size_t inlen, char ch62, char ch63, int pad)
{
    if(!ch62) ch62 = '+';
    if(!ch63) ch63 = '/';

    size_t len = 0;

    char buf[4];
    size_t nbufchars;

    size_t i, j;
    for(i = 0; i < inlen; i += 3)
    {
        /*if(i < inlen)*/
        nbufchars = 2;
        if(i+1 < inlen)
            nbufchars = 3;
        if(i+2 < inlen)
            nbufchars = 4;

        buf[0] = in[i] >> 2;
        buf[1] = in[i] << 4;
        if(i+1 < inlen)
        {
            buf[1] |= in[i+1] >> 4;
            buf[2] = in[i+1] << 2;
        }
        if(i+2 < inlen)
        {
            buf[2] |= in[i+2] >> 6;
            buf[3] = in[i+2];
        }

        for(j = 0; j < nbufchars; j++)
        {
            buf[j] &= 0x3F; /* 0b00111111 */

            if(buf[j] == 62) buf[j] = ch62;
            else if(buf[j] == 63) buf[j] = ch63;
            else buf[j] = TcodeBase64EncodeTable[(unsigned char)buf[j]];
        }
        if(pad)
            for(j = nbufchars; j < 4; j++)
                buf[j] = '=';

        len += _tcode_append(len, out, outlen, buf, 4);
    }
    out[MIN(outlen-1,len)] = 0;

    return len;
}
size_t tcode_base64decode(char* out, size_t outlen, const char* in, size_t inlen, int strict, char ch62, char ch63, int pad)
{
    if(!ch62) ch62 = '+';
    if(!ch63) ch63 = '/';

    size_t len = 0;

    char buf[4];
    size_t nbufchars;

    size_t i, j;
    for(i = 0; i < inlen; i += 4)
    {
    sloop:
        if(i+1 >= inlen)
        {
            if(strict)
                goto error;
            else
                break;
        }

        nbufchars = 1;
        if(i+2 < inlen)
            nbufchars = 2;
        if(i+3 < inlen)
            nbufchars = 3;

        if(strict && pad && nbufchars != 3)
            goto error;

        for(j = 0; j < nbufchars + 1; j++)
        {
            buf[j] = in[i+j];
            if(buf[j] == ch62) buf[j] = 62;
            else if(buf[j] == ch63) buf[j] = 63;
            else if(strchr(TcodeBase64EncodeTable, buf[j]))
                buf[j] = TcodeBase64DecodeTable[(unsigned char)buf[j]];
            else if(j > 2 && buf[j] == '=')
                buf[j] = 0;
            else if(strict)
                goto error;
            else /* skip unknown... */
            {
                i++;
                goto sloop;
            }
        }

        buf[0] = buf[0] << 2 | buf[1] >> 4;
        if(nbufchars >= 2)
            buf[1] = buf[1] << 4 | buf[2] >> 2;
        if(nbufchars >= 3)
            buf[2] = buf[2] << 6 | buf[3];

        len += _tcode_append(len, out, outlen, buf, nbufchars);
    }
    if(len < outlen)
        out[len] = 0; /* we only append 0 if we can as this is probably binary data anyways. */

    return len;
error:
    if(len < outlen)
        out[len] = 0; /* we only append 0 if we can as this is probably binary data anyways. */
    errno = EILSEQ;
    return 0;
}
