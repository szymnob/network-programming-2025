#include <stdio.h>
#include <stdbool.h>

bool is_printable_str(const char * str){
    
    if(str == NULL){
        return false;
    }

    while(*str){
        if(*str < 32 || *str > 126){
            return false;
        }

        str++;
    }

    return true;
}

int main(){
    char text1[] = "Tekst";
    char text2[] = "Tekst\n 2";


    printf("Text1: %s\n", is_printable_str(text1) ? "true" : "false");
    printf("Text2: %s\n", is_printable_str(text2) ? "true" : "false");

}