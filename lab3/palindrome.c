#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "palindrome.h"

int is_palindrome(const char *start, const char *end) {
    while (start < end) {
        if (tolower(*start) != tolower(*end)) {
            return 0;
        }
        start++;
        end--;
    }
    return 1;
}

const char * count_palindromes(const char *input) {
    int total_words = 0;
    int palindrome_count = 0;

    
    const char *word_start = NULL;
    int has_words = 0;

    for (const char *ptr = input; ; ptr++) {
        if (isalpha(*ptr)) {
            if (!word_start) {
                word_start = ptr;
            }
        } else {
            if (*ptr == ' ' && (ptr == input || *(ptr - 1) == ' ')) {
                return "ERROR\n";
            }
            if (word_start) {
                total_words++;
                if (is_palindrome(word_start, ptr - 1)) {
                    palindrome_count++;
                }
                word_start = NULL;
                has_words = 1;
            }
            if (*ptr == '\0') {
                break;
            }
        }
    }

    char *response = malloc(10);

    if (!has_words) {
        return "0/0\n";
    } else {
        snprintf(response, 10, "%d/%d\n", palindrome_count, total_words);
        return response;
    }
}
