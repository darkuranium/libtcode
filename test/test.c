#include <tcode/tcode.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

size_t strlen32(const tc_char32* str)
{
    size_t i;
    for(i = 0; str[i]; i++)
    {}
    return i;
}
size_t strlen16(const tc_char16* str)
{
    size_t i;
    for(i = 0; str[i]; i++)
    {}
    return i;
}

int main()
{
    char urlin[256] = "...and this is an ~URL~ % string...";
    char urlout[256];

    tcode_urlencode(urlout, sizeof(urlout), urlin, strlen(urlin), 1, 0);
    printf("%s\n%s\n\n", urlin, urlout);

    memcpy(urlin, urlout, sizeof(urlin));

    tcode_urldecode(urlout, sizeof(urlout), urlin, strlen(urlin), 1);
    printf("%s\n%s\n\n", urlin, urlout);

    char xmlin[256] = "an \"XML\"'s string <or so we say & that...>";
    char xmlout[256];

    tcode_xmlencode(xmlout, sizeof(xmlout), xmlin, strlen(xmlin), 1, 1);
    printf("%s\n%s\n\n", xmlin, xmlout);

    memcpy(xmlin, xmlout, sizeof(xmlin));

    tcode_xmldecode(xmlout, sizeof(xmlout), xmlin, strlen(xmlin), 1);
    printf("%s\n%s\n\n", xmlin, xmlout);

    char b64in[256] = "The quick brown fox jumped over the lazy dog.";
    char b64out[256];

    tcode_base64encode(b64out, sizeof(b64out), b64in, strlen(b64in), 0, 0, 1);
    printf("%s\n%s\n\n", b64in, b64out);

    memcpy(b64in, b64out, sizeof(b64in));

    tcode_base64decode(b64out, sizeof(b64out), b64in, strlen(b64in), 1, 0, 0, 1);
    printf("%s\n%s\n\n", b64in, b64out);

    tc_char8  u8in[256] = "This is some UTF-8 text... čšž... €50...";
    tc_char8  u8[256];
    tc_char16 u16[256];
    tc_char32 u32[256];
    
    tcode_utf8_to_utf32(u32, sizeof(u32) / sizeof(*u32), u8in, strlen(u8in), 1);
    tcode_utf32_to_utf16(u16, sizeof(u16) / sizeof(*u16), u32, strlen32(u32), 1);
    tcode_utf16_to_utf32(u32, sizeof(u32) / sizeof(*u32), u16, strlen16(u16), 1);
    tcode_utf32_to_utf8(u8, sizeof(u8) / sizeof(*u8), u32, strlen32(u32), 1);
    
    printf("%s\n%s\n\n", u8in, u8);

    return 0;
}
