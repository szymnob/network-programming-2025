#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 8

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <plik źródłowy> <plik docelowy>\n", argv[0]);
        exit(1);
    }

    int wejscie = open(argv[1], O_RDONLY);
    if (wejscie == -1) {
        perror("Błąd otwierania pliku źródłowego");
        exit(1);
    }

    int wyjscie = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (wyjscie == -1) {
        perror("Błąd otwierania pliku docelowego");
        close(wejscie);  // Zamykamy plik wejściowy, aby nie było wycieku zasobów
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(wejscie, buffer, BUFFER_SIZE)) > 0) {
        if (write(wyjscie, buffer, bytes_read) != bytes_read) {
            perror("Błąd zapisu do pliku");
            close(wejscie);
            close(wyjscie);
            exit(1);
        }
    }

    if (bytes_read == -1) {
        perror("Błąd odczytu pliku źródłowego");
        close(wejscie);
        close(wyjscie);
        exit(1);
    }

    if (close(wejscie) == -1) {
        perror("Błąd zamykania pliku źródłowego");
        exit(1);
    }
    if (close(wyjscie) == -1) {
        perror("Błąd zamykania pliku docelowego");
        exit(1);
    }

    printf("Plik '%s' został skopiowany do '%s'.\n", argv[1], argv[2]);
    return 0;
}
