#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

// current only support Decimal
static inline int itoa(int num, char *dest, int base) {
    int offset = 0;
    int rem = 0;
    char neg_flag = 0;
    
    if (num < 0) {
        neg_flag = 1;
        dest[offset++] = '-';
    }

    while (num != 0) {
        rem = neg_flag == 1 ? -(num % base) : num % base;
        dest[offset++] = '0' + rem;
        num /= base;
    }

    dest[offset] = '\0';

    char temp = 0;
    for (int i = 0 + neg_flag; i < ((offset + neg_flag) >> 1); ++i) {
        temp = dest[i];
        dest[i] = dest[offset - 1 - i + neg_flag];
        dest[offset - 1 - i + neg_flag] = temp;
    }

    // return length
    return offset;
}

int sprintf(char *out, const char *fmt, ...) {
    va_list ap;

    size_t fmt_index = 0;
    size_t out_index = 0;
    int cmp = 0;

    int integer = 0;
    int length_integer_str = 0;

    char * str = NULL;
    

    va_start(ap, fmt);
    char temp_buf[128] = {0};

    while (fmt[fmt_index] != '\0') {
        cmp = strncmp(fmt + fmt_index, "%d", 2);
        if (0 == cmp) {
            
            integer = va_arg(ap, int);
            length_integer_str = itoa(integer, temp_buf, 10);
            strcpy(out + out_index, temp_buf);
            
            out_index += length_integer_str;
            fmt_index += 2;

            continue;
        }

        cmp = strncmp(fmt + fmt_index, "%s", 2);
        if (0 == cmp) {
            
            str = va_arg(ap, char *);
            strcpy(out + out_index, str);
            out_index += strlen(str);
            fmt_index += 2;

            continue;
        }
        
        out[out_index] = fmt[fmt_index];
        ++out_index;
        ++fmt_index;
    }
    
    out[out_index] = '\0';

    return out_index;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  return 0;
}

#endif
