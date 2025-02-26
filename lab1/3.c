#include <stdio.h>
#include <stdbool.h>

bool is_printable_buf(const void * buf, int len){
    char * ptr = (char *) buf;

    for(int i = 0; i < len; i++){
        //printf("%c (%d)\n", ptr[i], ptr[i]);
        
        if(ptr[i] < 32 || ptr[i] > 126){
            return false;
        }
    }

    return true;
}

int main(){
    char text1[] = "Tekst";
    char text2[] = "Tekst\n 2";

    printf("Text1: %s\n", is_printable_buf(text1, sizeof(text1) - 1 ) ? "true" : "false");
    printf("Text2: %s\n", is_printable_buf(text2, sizeof(text2) - 1) ? "true" : "false");
}