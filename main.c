/** @file */
#include "funkcje.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (strcmp(argv[1],"-h")==0)
    {
        printf("%s", "------Pomoc------\n");
        printf("%s", "Poprawna ilosc i kolejnosc przelacznikow:\n");
        printf("%s", "Projekt.exe input.wav -przelacznik jegoWlasciwosci output\n");
        printf("%s", "Wlasciwosci przelacznikow w dokumentacji\n");
        printf("%s", "------Koniec------\n");
        return 0;
    }
    switch(checkFile(argc,argv))
    {
        case 1: printf("%s","Podano za malo argumentow programu, uruchom program z przelacznikiem -h aby wyswietlic pomoc");
            break;
        case 2: printf("%s","Bledne parametry programu, uruchom program z przelacznikiem -h aby wyswietlic pomoc");
            break;
        case 3: printf("%s","Podana wartosc nie jest liczba calkowita, uruchom program z przelacznikiem -h aby wyswietlic pomoc");
            break;
        case 4: printf("%s","Mapowanie probek tylko na 8 lub 16 bitow, uruchom program z przelacznikiem -h aby wyswietlic pomoc");
            break;
        default: break;
    }
}
