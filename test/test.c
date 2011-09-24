#include <tcode/tcode.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

    return 0;
}
