///** @file */
// Created by rfok7 on 15.04.2020.
//

#ifndef PROJEKT_FUNKCJE_H
#define PROJEKT_FUNKCJE_H
#include <inttypes.h>
#include <stdio.h>

/**
 *
 * @param filename Nazwa pliku wejściowego
 * @param outputFilename Nazwa pliku wyjściowego
 * @param outputSampling Wyjściowa wartość próbkowania
 * @param deleteLength Wartość zmiany długości pliku wyrażona w sekundach
 * @param outputBits Wyjściowa głębia bitowa dźwięku
 * @param whatToDo Flaga pomocniczna wykorzystywana do wywołania poprawnej funkcji
 */
void readHeader(char* filename,char* outputFilename,long int outputSampling, long int deleteLength ,int outputBits,int whatToDo);

/**
 *
 * @param argc Liczba podancyh parametrów przez użytkownika podczas wywołania programu
 * @param argv Tablica przechowująca argumenty w postaci tekstowej
 * @return Funkcja zwraca wartość (flagę), która wyświetla komunikaty błędu lub pomocy
 */
int checkFile(int argc, char **argv);


/**
 *
 * @param bufferHeader Tablica przechowująca nagłowek pliku .wav
 * @param sizeOfDataBuffer Rozmiar danych w pliku .wav
 * @param blockAlign Liczba bloków pamięci interpretowanych przez odtwarzacz jako pojedyncza próbka
 * @param channels Liczba kanałów
 * @param byteRate Strumień danych w bajtach na sekundę
 * @param outputSampling Częstotliwość próbkowania
 * @param outputBitsPerSample Rozdzielczość w bitach na próbkę (i kanał)
 */
void modifyHeader(unsigned char* bufferHeader, unsigned int sizeOfDataBuffer, uint32_t blockAlign, uint32_t channels,uint32_t byteRate,uint32_t outputSampling, uint32_t outputBitsPerSample);

/**
 *
 * @param pFile Wskaźnik na plik wejściowy
 * @param bufferHeader Tablica przechowująca nagłowek pliku .wav
 * @param outputFilename Nazwa pliku wyjściowego
 */
void monoToStereo(FILE *pFile,unsigned char* bufferHeader,char* outputFilename);

/**
 *
 * @param pFile Wskaźnik na plik wejściowy
 * @param bufferHeader Tablica przechowująca nagłowek pliku .wav
 * @param outputFilename Nazwa pliku wyjściowego
 */
void stereoToMono(FILE *pFile,unsigned char* bufferHeader,char* outputFilename);

/**
 *
 * @param pFile Wskaźnik na plik wejściowy
 * @param bufferHeader Tablica przechowująca nagłowek pliku .wav
 * @param outputFilename Nazwa pliku wyjściowego
 * @param outputSampling Wyjściowa wartość próbkowania
 */
void sampleRateChange(FILE * pFile,unsigned char* bufferHeader,char* outputFilename,double outputSampling);

/**
 *
 * @param pFile Wskaźnik na plik wejściowy
 * @param bufferHeader Tablica przechowująca nagłowek pliku .wav
 * @param outputFilename Nazwa pliku wyjściowego
 * @param outputBits Wyjściowa głębia bitowa dźwięku
 */
void bitsChange(FILE * pFile,unsigned char* bufferHeader,char* outputFilename, int outputBits);

/**
 *
 * @param pFile Wskaźnik na plik wejściowy
 * @param bufferHeader Tablica przechowująca nagłowek pliku .wav
 * @param outputFilename Nazwa pliku wyjściowego
 * @param secondsToChange Wartość zmiany długości pliku wyrażona w sekundach
 */
void lengthChange(FILE * pFile,unsigned char* bufferHeader,char* outputFilename, long int secondsToChange);

#endif //PROJEKT_FUNKCJE_H
