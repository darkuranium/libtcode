#include <tcode/internal.h>
#include <tcode/common.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h> 

static char errormsg[1024] = {0};

void tcode_error_raisefv(const char* format, va_list args)
{
    vsprintf(errormsg, format, args);
    errormsg[sizeof(errormsg) - 1] = 0;
    errno = EILSEQ;
}
void tcode_error_raisef(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    tcode_error_raisefv(format, args);
    va_end(args);
}
void tcode_error_raise(const char* msg)
{
    tcode_error_raisef("%s", msg);
}

char* tcode_error_getmsg(void)
{
    return errormsg;
}
void tcode_error_print(const char* prefix)
{
    if(prefix)
        fprintf(stderr, "%s: %s\n", prefix, errormsg);
    else
        fprintf(stderr, "%s\n", errormsg);
}
