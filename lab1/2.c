#include <stdio.h>

void drukuj_alt(int * tablica, int liczba_elementow){
    int liczba;

    for(int i = 0; i < liczba_elementow; i++){
        liczba = *tablica++;
        if (liczba > 10 && liczba <100){
            printf("%d\n", liczba);
        }
    }

}

int main(){
    int liczby[50];
    int buffer;
    int pointer = 0;

    while(1){
        scanf("%d", &buffer);

        if(buffer == 0){
            break;
        }

        liczby[pointer] = buffer;
        pointer++;
    }

    drukuj_alt(liczby, pointer);
}