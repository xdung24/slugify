#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "anyascii.h"

void append_ascii(char** result, size_t* len, size_t* cap, const char* str) {
    size_t slen = strlen(str);
    if (*len + slen >= *cap) {
        *cap = *cap * 2 + slen;
        *result = realloc(*result, *cap);
    }
    strcpy(*result + *len, str);
    *len += slen;
}

char* anyascii_full(const char* text) {
    char* result = malloc(1);
    result[0] = '\0';
    size_t len = 0;
    size_t cap = 1;
    const char* p = text;
    while (*p) {
        uint32_t codepoint = 0;
        int bytes = 0;
        if ((*p & 0x80) == 0) {
            codepoint = *p++;
            bytes = 1;
        } else if ((*p & 0xE0) == 0xC0) {
            codepoint = (*p++ & 0x1F) << 6;
            codepoint |= *p++ & 0x3F;
            bytes = 2;
        } else if ((*p & 0xF0) == 0xE0) {
            codepoint = (*p++ & 0x0F) << 12;
            codepoint |= (*p++ & 0x3F) << 6;
            codepoint |= *p++ & 0x3F;
            bytes = 3;
        } else if ((*p & 0xF8) == 0xF0) {
            codepoint = (*p++ & 0x07) << 18;
            codepoint |= (*p++ & 0x3F) << 12;
            codepoint |= (*p++ & 0x3F) << 6;
            codepoint |= *p++ & 0x3F;
            bytes = 4;
        } else {
            // Invalid, skip
            p++;
            continue;
        }
        const char* ascii_str = NULL;
        size_t ascii_len = anyascii(codepoint, &ascii_str);
        if (ascii_len > 0 && ascii_str) {
            append_ascii(&result, &len, &cap, ascii_str);
        }
    }
    return result;
}

char* slugify(const char* text) {
    char* ascii = anyascii_full(text);
    if (!ascii) return NULL;
    // lowercase
    for (char* p = ascii; *p; p++) *p = tolower(*p);
    // replace
    char* result = malloc(strlen(ascii) + 1);
    char* r = result;
    int last_sep = 1;
    for (char* p = ascii; *p; p++) {
        if (isalnum(*p)) {
            *r++ = *p;
            last_sep = 0;
        } else if (!last_sep && *p != '\n' && *p != '\r') {  // avoid replacing newlines
            *r++ = '-';
            last_sep = 1;
        }
    }
    if (r > result && *(r-1) == '-') r--;
    *r = '\0';
    free(ascii);
    return result;
}

int main() {
    // read all stdin
    char* input = NULL;
    size_t len = 0;
    size_t cap = 0;
    char buf[1024];
    while (fgets(buf, sizeof(buf), stdin)) {
        size_t blen = strlen(buf);
        if (len + blen >= cap) {
            cap = cap * 2 + blen + 1;
            input = realloc(input, cap);
        }
        strcpy(input + len, buf);
        len += blen;
    }
    if (input) {
        char* output = slugify(input);
        if (output) {
            printf("%s\n", output);
            free(output);
        }
        free(input);
    }
    return 0;
}