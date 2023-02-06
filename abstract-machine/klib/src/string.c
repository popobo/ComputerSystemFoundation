#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    size_t offset = 0;
    while (*(s + offset) != '\0') {
        ++offset;
    }
    return offset;
}

char *strcpy(char* dst,const char* src) {
    size_t offset = 0;

    while ((*(dst + offset) = *(src + offset)) != '\0') {
        ++offset;
    }
    
    return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
    size_t offset;

    for (offset = 0; offset < n && src[offset] != '\0'; ++offset) {
        dst[offset] = src[offset];
    }

    for (; offset < n; ++offset) {
        dst[offset] = '\0';
    }

    return dst;
}

char* strcat(char* dst, const char* src) {
    size_t dst_offset = strlen(dst);
    size_t src_offset = 0;
    while ((dst[dst_offset] = src[src_offset]) != '\0') {
        ++dst_offset;
        ++src_offset;
    }
    return dst;
}

int strcmp(const char* s1, const char* s2) {
    size_t offset = 0;
    int result = 0;
    while (s1[offset] != '\0' && s2[offset] != '\0') {
        result = s1[offset] - s2[offset];
        if (result != 0) {
            break;
        }
        offset++;
    }

    result = s1[offset] - s2[offset];

    return result;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    int result = 0;
    size_t i = 0;
    for (i = 0; i < n && s1[i] != '\0' && s2[i] != 0; ++i) {
        result = s1[i] - s2[i];
        if (result != 0) {
            break;
        }
    }

    if ((i != n) && (s1[i] == '\0' || s2[i] == '\0')) {
        result = s1[i] - s2[i];
    }

    return result;
}

void* memset(void* v,int c,size_t n) {
    char *iv = (char *)v;
    for (int i = 0; i < n; ++i) {
        iv[i] = c;
    }
    return v;
}

void* memmove(void* dst,const void* src,size_t n) {
    char *cdst = (char *)dst;
    char *csrc = (char *)src;
    char * temp = (char *)malloc(sizeof(char) * n);
    if (NULL == temp) {
        return NULL;
    }

    size_t i = 0;

    for (i = 0; i < n; ++i) {
        temp[i] = csrc[i];
    }

    for (i = 0; i < n; ++i) {
        cdst[i] = temp[i];
    }
    free(temp);

    return dst;
}

void* memcpy(void* dst, const void* src, size_t n) {
    char *cdst = (char *)dst;
    char *csrc = (char *)src;
    
    if (src + n > dst) {
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        cdst[i] = csrc[i];
    }
    return dst;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    char *cs1 = (char *)s1;
    char *cs2 = (char *)s2;
    int result = 0;

    for (int i = 0; i < n; ++i) {
        result = cs1[i] - cs2[i];
        if (result != 0) {
            break;
        }
    }

    return result;
}

#endif
