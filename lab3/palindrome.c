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

void count_palindromes(const char *input, char *result) {
    int total_words = 0;
    int palindrome_count = 0;

    
    const char *word_start = NULL;
    int has_words = 0;

    for (const char *ptr = input; ; ptr++) {
        //check for invalid characters
        if(!isalpha(*ptr) && *ptr != ' ' && *ptr != '\0'){
            strcpy(result, "ERROR");
            return;
        }
        if (isalpha(*ptr)) {
            if (!word_start) {
                word_start = ptr;
            }
        } else {
            if (*ptr == ' ' && *(ptr - 1) == ' ') {
                strcpy(result, "ERROR");
                return;
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


    if (!has_words) {
        strcpy(result, "0/0");
        return;
    } else {
        snprintf(result, RESPONSE_SIZE, "%d/%d", palindrome_count, total_words);
    }
}
