#include "mini_utf8.h"

#ifndef NO_UTF8

uint16_t utf8_getchar(const char **str)
{
    uint8_t c;
    uint8_t tmp, seqlen;
    uint16_t result;
    
    c = **str;
    if (!c)
        return 0;
    
    (*str)++;
    
    if ((c & 0x80) == 0)
    {
        /* Just normal ASCII character. */
        return c;
    }
    else if ((c & 0xC0) == 0x80)
    {
        /* Dangling piece of corrupted multibyte sequence.
         * Did you cut the string in the wrong place?
         */
        return c;
    }
    else if ((**str & 0xC0) == 0xC0)
    {
        /* Start of multibyte sequence without any following bytes.
         * Silly. Maybe you are using the wrong encoding.
         */
        return c;
    }
    else
    {
        /* Beginning of a multi-byte sequence.
         * Find out how many characters and combine them.
         */
        seqlen = 2;
        tmp = 0x20;
        result = 0;
        while ((c & tmp) && (seqlen < 5))
        {
            seqlen++;
            tmp >>= 1;
            
            result = (result << 6) | (**str & 0x3F);
            (*str)++;
        }
        
        result = (result << 6) | (**str & 0x3F);
        (*str)++;
        
        result |= (c & (tmp - 1)) << ((seqlen - 1) * 6);
        return result;
    }
}

void utf8_rewind(const char **str)
{
    (*str)--;
    
    while ((**str & 0x80) != 0x00 && (**str & 0xC0) != 0xC0)
        (*str)--;
}

#else

/* This is 8-bit alternative if you don't need UTF-8. */
static uint16_t utf8_getchar(const char **str)
{
    if (**str)
        return *(*str++);
    else
        return 0;
}

void utf8_rewind(const char **str)
{
    (*str)--;
}

#endif