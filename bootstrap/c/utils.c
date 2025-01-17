#ifndef __UTILS_C__
#define __UTILS_C__

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// intiger types
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef ssize_t  isize;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t   usize;

typedef unsigned char uchar;
typedef uchar *cString;

usize cstr_len(cString str) {
    usize len;
    
    len = 0;
    while (str[len]) {
        len++;
    }

    return len;
}

usize flen(FILE *pFile) {
    usize size;
    
    fseek(pFile, 0, SEEK_END);
    size = ftell(pFile);
    rewind(pFile);

    return size;
}

#endif // __UTILS_C__
