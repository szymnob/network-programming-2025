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



    //check if space in the beginning
    if (input[0] == ' ') {
        strcpy(result, "ERROR\n");
        return;
    }

    // //check if space on the end
    // size_t len = strlen(input);
    // if (len > 0) {
    //     const char *end = input + len - 1;

    //     while (end >= input && (*end == '\n' || *end == '\r')) {
    //         end--;
    //     }

    //     if (*end == ' ') {
    //         strcpy(result, "ERROR\n");
    //         return;
    //     }
    // }

    for (const char *ptr = input; ; ptr++) {
        //check for digit value
        if(isdigit(*ptr)){
            strcpy(result, "ERROR\n");
            return;
        }
        if (isalpha(*ptr)) {
            if (!word_start) {
                word_start = ptr;
            }
        } else {
            if (*ptr == ' ' && *(ptr - 1) == ' ') {
                strcpy(result, "ERROR\n");
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
        strcpy(result, "0/0\n");
        return;
    } else {
        snprintf(result, RESPONSE_SIZE, "%d/%d\n", palindrome_count, total_words);
    }
}
