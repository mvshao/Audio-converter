/** @file */
//
// Created by rfok7 on 15.04.2020.
//

/*!
   Minimalna liczba argumentow przekazanych do programu
*/
#define MIN_NUM_OF_ARGUMENTS 4

/*!
   Stały rozmiar nagłówka pliku .wav
*/
#define SIZE_OF_WAV_HEADER 44

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "funkcje.h"

int checkFile(int argc, char **argv)
{
    char* filename;
    char* outputFilename;

    if (argc>=MIN_NUM_OF_ARGUMENTS)
    {
        filename = argv[1];
        if (strcmp(argv[2],"-ms")==0)
        {
            outputFilename = argv[3];
            readHeader(filename,outputFilename,0,0,0, 1);
        }
        else if (strcmp(argv[2],"-sm")==0)
        {
            outputFilename = argv[3];
            readHeader(filename,outputFilename,0,0,0, 2);
        }
        else if (strcmp(argv[2],"-zb")==0 && argc==5)
        {
            outputFilename = argv[4];
            int outputBits = atoi(argv[3]);
            if((outputBits == 16 || outputBits  == 8 ))
                readHeader(filename,outputFilename,0,0,outputBits,4);
            else
                return 4;
        }
        else if (strcmp(argv[2],"-cp")==0 && argc==5)
        {
            //sprawdzanie czy podana liczba jest liczba calkowita
            long int valueInt = strtol(argv[3],&argv[3],0);
            double valueDouble = strtod(argv[3],&argv[3]);
                if(valueDouble==0)
                {
                    outputFilename = argv[4];
                    readHeader(filename,outputFilename,valueInt,0,0, 3);
                }
                else
                    return 3; //nie jest liczba calkowita
        }
        else if (strcmp(argv[2],"-zd")==0 && argc==5)
            {
                long int valueDelete = strtol(argv[3],&argv[3],0);
                if(argc<6)
                {
                    outputFilename = argv[4];
                    readHeader(filename,outputFilename,0,valueDelete, 0,5);
                }
            }
        else
            return 2;//nie ma takiego przelacznika
    }
    else
        return 1; //za malo parametrow
    return 0;
}

void readHeader(char* filename,char* outputFilename,long int outputSampling, long int changeLength ,int outputBits,int whatToDo)
{
    unsigned char* bufferHeader;
    bufferHeader =  malloc(SIZE_OF_WAV_HEADER);
    FILE *pFile;
    pFile = fopen(filename,"rb");
    if(pFile !=NULL)
    {
        fread(bufferHeader,  SIZE_OF_WAV_HEADER, 1, pFile);
        switch (whatToDo)
        {
            case 1: monoToStereo(pFile,bufferHeader,outputFilename);
                break;
            case 2: stereoToMono(pFile,bufferHeader,outputFilename);
                break;
            case 3: sampleRateChange(pFile,bufferHeader,outputFilename,outputSampling);
                break;
            case 4: bitsChange(pFile,bufferHeader,outputFilename,outputBits);
                break;
            case 5: lengthChange(pFile,bufferHeader,outputFilename,changeLength);
                break;
            default: return;
        }
        fclose(pFile);
    }
    free(bufferHeader);
}

void modifyHeader(unsigned char* bufferHeader, unsigned int sizeOfDataBuffer, uint32_t blockAlign, uint32_t channels,uint32_t byteRate,uint32_t outputSampling, uint32_t outputBitsPerSample)
{
    uint8_t datasizeAfterConv[4];
    uint8_t channelsAfterConv[2];
    uint8_t blockAlignAfterConv[2];
    uint8_t byteRateAfterConv[4];
    uint8_t sampleRateAfterConv[4];
    uint8_t bitsPerSampleAfterConv[2];

    blockAlignAfterConv[0] = (uint8_t)(blockAlign >> 0u);
    blockAlignAfterConv[1] = (uint8_t)(blockAlign >> 8u);
    bufferHeader[32] = blockAlignAfterConv[0];
    bufferHeader[33] = blockAlignAfterConv[1];

    byteRateAfterConv[0] = (uint8_t)(byteRate >> 0u);
    byteRateAfterConv[1] = (uint8_t)(byteRate >> 8u);
    byteRateAfterConv[2] = (uint8_t)(byteRate >> 16u);
    byteRateAfterConv[3] = (uint8_t)(byteRate >> 24u);
    bufferHeader[28] = byteRateAfterConv[0];
    bufferHeader[29] = byteRateAfterConv[1];
    bufferHeader[30] = byteRateAfterConv[2];
    bufferHeader[31] = byteRateAfterConv[3];

    channelsAfterConv[0] = (uint8_t)(channels>>0u);
    channelsAfterConv[1] = (uint8_t)(channels>>8u);
    bufferHeader[22] = channelsAfterConv[0];
    bufferHeader[23] = channelsAfterConv[1];

    datasizeAfterConv[0] = (uint8_t)(sizeOfDataBuffer >> 0u);
    datasizeAfterConv[1] = (uint8_t)(sizeOfDataBuffer >> 8u);
    datasizeAfterConv[2] = (uint8_t)(sizeOfDataBuffer >> 16u);
    datasizeAfterConv[3] = (uint8_t)(sizeOfDataBuffer >> 24u);
    bufferHeader[40] = datasizeAfterConv[0];
    bufferHeader[41] = datasizeAfterConv[1];
    bufferHeader[42] = datasizeAfterConv[2];
    bufferHeader[43] = datasizeAfterConv[3];

    sampleRateAfterConv[0] = (uint8_t)(outputSampling >> 0u);
    sampleRateAfterConv[1] = (uint8_t)(outputSampling >> 8u);
    sampleRateAfterConv[2] = (uint8_t)(outputSampling >> 16u);
    sampleRateAfterConv[3] = (uint8_t)(outputSampling >> 24u);
    bufferHeader[24] = sampleRateAfterConv[0];
    bufferHeader[25] = sampleRateAfterConv[1];
    bufferHeader[26] = sampleRateAfterConv[2];
    bufferHeader[27] = sampleRateAfterConv[3];

    bitsPerSampleAfterConv[0] = (uint8_t)(outputBitsPerSample >> 0u);
    bitsPerSampleAfterConv[1] = (uint8_t)(outputBitsPerSample >> 8u);
    bufferHeader[34] = bitsPerSampleAfterConv[0];
    bufferHeader[35] = bitsPerSampleAfterConv[1];
}

void monoToStereo(FILE * pFile,unsigned char* bufferHeader,char* outputFilename)
{
    //konwersja little-endian do big-endian
    const uint32_t bitsPerSample = bufferHeader[34] |
                                   (bufferHeader[35]<<8u);

    const uint32_t channels  = bufferHeader[22] |
                                  (bufferHeader[23]<<8u);

    const uint32_t sizeData = bufferHeader[40] |
                                (bufferHeader[41]<<8u) |
                                (bufferHeader[42]<<16u)|
                                (bufferHeader[43]<<24u);

    const int32_t originalSampling = bufferHeader[24] |
                               (bufferHeader[25]<<8u) |
                               (bufferHeader[26]<<16u) |
                               (bufferHeader[27]<<24u);

    FILE * pOutputFile;
    unsigned char* bufferData;
    unsigned char* bufferDataAfterChange;
    unsigned int sizeOfDataBuffer = sizeData;

    bufferData = malloc(sizeOfDataBuffer);


    if (!fread(bufferData, sizeOfDataBuffer,1,pFile))
        return;


    if(channels == 1)
    {
        bufferDataAfterChange = malloc(2*sizeOfDataBuffer);
        if(bitsPerSample==8)
        {
           pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");

            int k=0;
            for (int i=0; i<sizeOfDataBuffer;i++)
            {
                for (int j=0; j < 1; j++)
                {
                    bufferDataAfterChange[k] = bufferData[i];
                    bufferDataAfterChange[k+1] = bufferData[i];
                }
                k+=2;
            }

            modifyHeader(bufferHeader, 2*sizeOfDataBuffer,2,2,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
            fwrite(bufferDataAfterChange,(2*sizeOfDataBuffer),1,pOutputFile);
            fclose(pOutputFile);
        }
        else if(bitsPerSample==16)
        {
            pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");

            int k=0;
            for (int i=0; i<sizeOfDataBuffer;i+=2)
            {
                for (int j=0; j < 1; j++)
                {
                    bufferDataAfterChange[k] = bufferData[i];
                    bufferDataAfterChange[k+1] = bufferData[i+1];
                    bufferDataAfterChange[k+2] = bufferData[i];
                    bufferDataAfterChange[k+3] = bufferData[i+1];
                }
                k+=4;
            }
            modifyHeader(bufferHeader, 2*sizeOfDataBuffer,4,2,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
            fwrite(bufferDataAfterChange,(2*sizeOfDataBuffer),1,pOutputFile);
            fclose(pOutputFile);
        }
        free(bufferDataAfterChange);
    }
    free(bufferData);
}

void stereoToMono(FILE *pFile,unsigned char* bufferHeader,char* outputFilename)
{

    //konwersja little-endian do big-endian
    const uint32_t bitsPerSample = bufferHeader[34] |
                                   (bufferHeader[35]<<8u);

    const uint32_t channels  = bufferHeader[22] |
                               (bufferHeader[23]<<8u);

    const uint32_t sizeData = bufferHeader[40] |
                              (bufferHeader[41]<<8u) |
                              (bufferHeader[42]<<16u)|
                              (bufferHeader[43]<<24u);

    const int32_t originalSampling = bufferHeader[24] |
                                     (bufferHeader[25]<<8u) |
                                     (bufferHeader[26]<<16u) |
                                     (bufferHeader[27]<<24u);


    FILE * pOutputFile;
    unsigned char* bufferData;
    unsigned char* bufferDataAfterChange;
    unsigned int sizeOfDataBuffer = sizeData;

    bufferData = malloc(sizeOfDataBuffer);

    if (!fread(bufferData, sizeOfDataBuffer,1,pFile))
        return;

    if(channels == 2)
    {
        bufferDataAfterChange = malloc(sizeOfDataBuffer/2);
        pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");
        if (bitsPerSample == 8) {
            int k = 0;
            for (int i = 0; i < sizeOfDataBuffer / 2; ++i) {
                for (int j = 0; j < 1; ++j) {
                    bufferDataAfterChange[i] = (bufferData[k] + bufferData[k + 1]) / 2;
                }
                k += 2;
            }
            modifyHeader(bufferHeader, sizeOfDataBuffer/2,1,1,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            fwrite(bufferHeader, SIZE_OF_WAV_HEADER, 1, pOutputFile);
            fwrite(bufferDataAfterChange, (sizeOfDataBuffer / 2), 1, pOutputFile);
            fclose(pOutputFile);
        } else if (bitsPerSample == 16) {

            int k = 0;
            for (int i = 0; i < sizeOfDataBuffer/2; i+=2) {
                for (int j = 0; j < 1; ++j) {
                    /* If the mix of your input file is stereo mixing them even using a analog circuit will give a poor quality.
                     * If the mix is not a real stereo (i.e channel left and right are the same) then their will be no impact at all,
                     * but mixing the different values for left and right channel will give a poor quality..*/
                    bufferDataAfterChange[i]= (bufferData[k] + bufferData[k+2]) / 2;
                    bufferDataAfterChange[i+1]= (bufferData[k+1] + bufferData[k+3]) / 2;
                }
                k += 4;
            }
            modifyHeader(bufferHeader, sizeOfDataBuffer/2,2,1,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            fwrite(bufferHeader, SIZE_OF_WAV_HEADER, 1, pOutputFile);
            fwrite(bufferDataAfterChange, (sizeOfDataBuffer / 2), 1, pOutputFile);
            fclose(pOutputFile);
        }
        free(bufferDataAfterChange);
    }
    free(bufferData);
}

void sampleRateChange(FILE * pFile,unsigned char* bufferHeader,char* outputFilename,double outputSampling)
{
    //konwersja little-endian do big-endian
    const uint32_t bitsPerSample = bufferHeader[34] |
                                   (bufferHeader[35]<<8u);

    const uint32_t channels  = bufferHeader[22] |
                               (bufferHeader[23]<<8u);

    const uint32_t sizeData = bufferHeader[40] |
                              (bufferHeader[41]<<8u) |
                              (bufferHeader[42]<<16u)|
                              (bufferHeader[43]<<24u);

    const int32_t originalSampling = bufferHeader[24] |
                                     (bufferHeader[25]<<8u) |
                                     (bufferHeader[26]<<16u) |
                                     (bufferHeader[27]<<24u);

    FILE * pOutputFile;
    uint32_t tempData;
    unsigned char* bufferData;
    unsigned char* bufferDataAfterChange;
    unsigned int sizeOfDataBuffer = sizeData;
    int k=0;

    bufferData = malloc(sizeOfDataBuffer);

    if (!fread(bufferData, sizeOfDataBuffer,1,pFile))
        return;

    if(originalSampling >= outputSampling)
    {
        double checkDouble = originalSampling / outputSampling ;
        int checkInt = (int)checkDouble;
        if(checkInt!=checkDouble)
            return;

        int difference = log2(originalSampling / outputSampling);

        //sprawdzenie czy outputSampling dzieli sie na dwa, czy difference jest liczba calkowita
        pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");

        if(channels == 1)
        {
            if(bitsPerSample==8)
            {
                int i;
                for (i=0; i < difference; ++i) {
                    for (int j = 0; j < sizeOfDataBuffer / pow(2,i); j+=2) {
                        bufferData[k] = (bufferData[j] + bufferData[j+1]) / 2;
                        k+=1;
                    }
                    k=0;
                }
                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer / pow(2, i)), 1,1,(outputSampling*1*(bitsPerSample/8)),outputSampling,bitsPerSample);
                fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
                fwrite(bufferData,ceil(sizeOfDataBuffer / pow(2, i)),1,pOutputFile);
                fclose(pOutputFile);
            }

            else if (bitsPerSample==16)
            {
                int i;
                for (i=0; i < difference; ++i) {
                    for (int j = 0; j < sizeOfDataBuffer / pow(2,i); j+=4) {
                        tempData = ((bufferData[j] | bufferData[j+1]<<8u) + (bufferData[j+2] | bufferData[j+3]<<8u))/2;
                        bufferData[k] = (uint8_t)tempData;
                        bufferData[k+1] = (uint8_t)(tempData >> 8u);
                        k+=2;
                    }
                    k=0;
                }
                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer / pow(2, i)), 2,1,(outputSampling*1*(bitsPerSample/8)),outputSampling,bitsPerSample);
                fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
                fwrite(bufferData,ceil(sizeOfDataBuffer / pow(2, i)),1,pOutputFile);
                fclose(pOutputFile);
            }
        }
        else if (channels == 2 )
        {
            if(bitsPerSample==8)
            {
                int i;
                for (i=0; i < difference; ++i) {
                    for (int j = 0; j < sizeOfDataBuffer / pow(2,i); j+=4) {
                        tempData = ((bufferData[j] | bufferData[j+1]<<8u) + (bufferData[j+2] | bufferData[j+3]<<8u))/2;
                        bufferData[k] = (uint8_t)tempData;
                        bufferData[k+1] = (uint8_t)(tempData >> 8u);
                        k+=2;
                    }
                    k=0;
                }
                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer / pow(2, i)), 2,2,(outputSampling*2*(bitsPerSample/8)),outputSampling,bitsPerSample);
                fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
                fwrite(bufferData,ceil(sizeOfDataBuffer / pow(2, i)),1,pOutputFile);
                fclose(pOutputFile);
            }
            else if (bitsPerSample == 16)
            {
                int i;
                for (i=0; i < difference; ++i) {
                    for (int j = 0; j < sizeOfDataBuffer / pow(2,i); j+=8) {
                        tempData = ((bufferData[j] | bufferData[j+1]<<8u | bufferData[j+2]<<16u | bufferData[j+3]<<24u ) + (bufferData[j+4] | bufferData[j+5]<<8u | bufferData[j+6]<<16u | bufferData[j+7]<<24u))/2;
                        bufferData[k] = (uint8_t)tempData;
                        bufferData[k+1] = (uint8_t)(tempData >> 8u);
                        bufferData[k+2] = (uint8_t)(tempData >> 16u);
                        bufferData[k+3] = (uint8_t)(tempData >> 24u);
                        k+=4;
                    }
                    k=0;
                }
                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer / pow(2, i)), 4,2,(outputSampling*2*(bitsPerSample/8)),outputSampling,bitsPerSample);
                fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
                fwrite(bufferData,ceil(sizeOfDataBuffer / pow(2, i)),1,pOutputFile);
                fclose(pOutputFile);
            }
        }

    } else
    {
        double checkDouble = outputSampling / originalSampling;
        int checkInt = (int)checkDouble;
        if(checkInt!=checkDouble)
            return;

        int difference = log2(outputSampling / originalSampling );

        pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");
        bufferDataAfterChange = malloc(sizeOfDataBuffer*pow(2,difference));
        if (channels == 1) {
            if (bitsPerSample == 8) {
                for (int i = 0; i < sizeOfDataBuffer; ++i) {
                    for (int j = 0; j <= difference; ++j) {
                        bufferDataAfterChange[k] = bufferData[i];
                        k += 1;
                    }
                }

                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer * pow(2, difference)), 1, 1,(outputSampling * 1 * (bitsPerSample / 8)), outputSampling,bitsPerSample);
                fwrite(bufferHeader, SIZE_OF_WAV_HEADER, 1, pOutputFile);
                fwrite(bufferDataAfterChange, ceil(sizeOfDataBuffer * pow(2, difference)), 1, pOutputFile);
                fclose(pOutputFile);
            }
            else if (bitsPerSample == 16) {
                for (int i = 0; i < sizeOfDataBuffer; i+=2) {
                    for (int j = 0; j <= difference; ++j) {
                        bufferDataAfterChange[k] = bufferData[i];
                        bufferDataAfterChange[k+1] = bufferData[i+1];
                        k+=2;
                    }
                }

                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer * pow(2, difference)), 2, 1,(outputSampling * 1 * (bitsPerSample / 8)), outputSampling,bitsPerSample);
                fwrite(bufferHeader, SIZE_OF_WAV_HEADER, 1, pOutputFile);
                fwrite(bufferDataAfterChange, ceil(sizeOfDataBuffer * pow(2, difference)), 1, pOutputFile);
                fclose(pOutputFile);
            }
        }
        else if (channels == 2)
        {
            if (bitsPerSample == 8)
            {
                for (int i = 0; i < sizeOfDataBuffer; i+=2) {
                    for (int j = 0; j <= difference; ++j) {
                        bufferDataAfterChange[k] = bufferData[i];
                        bufferDataAfterChange[k+1] = bufferData[i+1];
                        k+=2;
                    }
                }

                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer * pow(2, difference)), 2, 2,(outputSampling * 2 * (bitsPerSample / 8)), outputSampling,bitsPerSample);
                fwrite(bufferHeader, SIZE_OF_WAV_HEADER, 1, pOutputFile);
                fwrite(bufferDataAfterChange, ceil(sizeOfDataBuffer * pow(2, difference)), 1, pOutputFile);
                fclose(pOutputFile);
            }
            else if (bitsPerSample ==16)
            {
                for (int i = 0; i < sizeOfDataBuffer; i+=4) {
                    for (int j = 0; j <= difference; ++j) {
                        bufferDataAfterChange[k] = bufferData[i];
                        bufferDataAfterChange[k+1] = bufferData[i+1];
                        bufferDataAfterChange[k+2] = bufferData[i+2];
                        bufferDataAfterChange[k+3] = bufferData[i+3];
                        k+=4;
                    }
                }

                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer * pow(2, difference)), 4, 2,(outputSampling * 2 * (bitsPerSample / 8)), outputSampling,bitsPerSample);
                fwrite(bufferHeader, SIZE_OF_WAV_HEADER, 1, pOutputFile);
                fwrite(bufferDataAfterChange, ceil(sizeOfDataBuffer * pow(2, difference)), 1, pOutputFile);
                fclose(pOutputFile);
            }
        }
        free(bufferDataAfterChange);
    }
    free(bufferData);
}

void bitsChange(FILE * pFile,unsigned char* bufferHeader,char* outputFilename,int outputBits)
{
    const uint32_t bitsPerSample = bufferHeader[34] |
                                   (bufferHeader[35]<<8u);

    const uint32_t channels  = bufferHeader[22] |
                               (bufferHeader[23]<<8u);

    const uint32_t sizeData = bufferHeader[40] |
                              (bufferHeader[41]<<8u) |
                              (bufferHeader[42]<<16u)|
                              (bufferHeader[43]<<24u);

    const int32_t originalSampling = bufferHeader[24] |
                                     (bufferHeader[25]<<8u) |
                                     (bufferHeader[26]<<16u) |
                                     (bufferHeader[27]<<24u);

    FILE * pOutputFile;
    unsigned char* bufferData;
    unsigned char* bufferDataAfterChange;
    unsigned int sizeOfDataBuffer = sizeData;
    uint8_t sample8bit;
    int16_t sample16bit;
    int k=0;
    int j = 0;
    bufferData = malloc(sizeOfDataBuffer);

    if (!fread(bufferData, sizeOfDataBuffer,1,pFile))
        return;

    if(outputBits != bitsPerSample)
    {
        if (outputBits == 8)
        {
            // tutaj mam do czynienia z plikiem 16 bitowym
            bufferDataAfterChange = malloc(sizeOfDataBuffer/2);
            if (channels == 2)
            {
                pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");
                for (int i =0; i < sizeOfDataBuffer / 4; i++)
                {
                    sample16bit = bufferData[j] | (bufferData[j+1]<<8u);
                    sample8bit = sample16bit >> 8u;
                    sample8bit += (sample16bit & 0x80)>>7u;
                    bufferDataAfterChange[k] = sample8bit;

                    sample16bit = bufferData[j+2] | (bufferData[j+3]<<8u);
                    sample8bit = sample16bit >> 8u;
                    sample8bit += (sample16bit & 0x80)>>7u;
                    bufferDataAfterChange[k+1] = sample8bit;

                    j+=4;
                    k+=2;
                }
                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer / 2), 2,2,(originalSampling*channels*(outputBits/8)),originalSampling,outputBits);
                fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
                fwrite(bufferDataAfterChange,ceil(sizeOfDataBuffer/2),1,pOutputFile);
                fclose(pOutputFile);
            }
            else
            {
                pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");
                for (int i=0; i < sizeOfDataBuffer / 2; i++)
                {
                    sample16bit = bufferData[j] | (bufferData[j+1]<<8u);
                    sample8bit = sample16bit >> 8u;
                    sample8bit += (sample16bit & 0x80)>>7u;
                    bufferDataAfterChange[i] = sample8bit;
                    j+=2;
                }
                modifyHeader(bufferHeader, ceil(sizeOfDataBuffer / 2), 1,1,(originalSampling*channels*(outputBits/8)),originalSampling,outputBits);
                fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
                fwrite(bufferDataAfterChange,ceil(sizeOfDataBuffer/2),1,pOutputFile);
                fclose(pOutputFile);
            }
            free(bufferDataAfterChange);
        }
        else if (outputBits == 16)
        {
            // tutaj mam do czynienia z plikiem 8 bitowym
            bufferDataAfterChange = malloc(sizeOfDataBuffer*2);
            if (channels == 2)
            {
                pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");
                for (int i =0; i < sizeOfDataBuffer; i+=2)
                {
                    sample8bit = bufferData[j];
                    sample16bit = (uint16_t)(sample8bit - 0x80) << 8u;
                    bufferDataAfterChange[k] = sample16bit;
                    bufferDataAfterChange[k+1] = sample16bit >> 8u;

                    sample8bit = bufferData[j+1];
                    sample16bit = (uint16_t)(sample8bit - 0x80) << 8u;
                    bufferDataAfterChange[k+2] = sample16bit;
                    bufferDataAfterChange[k+3] = sample16bit >> 8u;

                    j+=2;
                    k+=4;
                }
                modifyHeader(bufferHeader, sizeOfDataBuffer * 2, 4,2,(originalSampling*channels*(outputBits/8)),originalSampling,outputBits);
                fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
                fwrite(bufferDataAfterChange,sizeOfDataBuffer*2,1,pOutputFile);
                fclose(pOutputFile);
            }
            else
            {
                pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");
                for (int i =0; i < sizeOfDataBuffer; i++)
                {
                    sample8bit = bufferData[i];
                    sample16bit = (uint16_t)(sample8bit -  0x80) << 8u;
                    bufferDataAfterChange[k] = sample16bit;
                    bufferDataAfterChange[k+1] = sample16bit >>8u;

                    k+=2;
                }
                modifyHeader(bufferHeader, sizeOfDataBuffer * 2, 2,1,(originalSampling*channels*(outputBits/8)),originalSampling,outputBits);
                fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
                fwrite(bufferDataAfterChange,sizeOfDataBuffer*2,1,pOutputFile);
                fclose(pOutputFile);
            }
            free(bufferDataAfterChange);
        }
    }
    free(bufferData);
}

void lengthChange(FILE * pFile,unsigned char* bufferHeader,char* outputFilename, long int secondsToChange)
{
    const uint32_t bitsPerSample = bufferHeader[34] |
                                   (bufferHeader[35]<<8u);

    const uint32_t channels  = bufferHeader[22] |
                               (bufferHeader[23]<<8u);

    const uint32_t sizeData = bufferHeader[40] |
                              (bufferHeader[41]<<8u) |
                              (bufferHeader[42]<<16u)|
                              (bufferHeader[43]<<24u);

    const uint32_t originalSampling = bufferHeader[24] |
                                     (bufferHeader[25]<<8u) |
                                     (bufferHeader[26]<<16u) |
                                     (bufferHeader[27]<<24u);

    FILE * pOutputFile;
    unsigned char* bufferData;
    unsigned char* bufferDataAfterChange;
    unsigned int sizeOfDataBuffer = sizeData;
    const int bitRate = originalSampling * channels * (bitsPerSample/8);

    bufferData =  malloc(sizeOfDataBuffer);

    if (!fread(bufferData, sizeOfDataBuffer,1,pFile))
        return;

    if (secondsToChange > 0)
    {
        unsigned long int bitsToAdd = bitRate * secondsToChange;
        bufferDataAfterChange =  malloc(sizeOfDataBuffer + bitsToAdd);
        memmove(bufferDataAfterChange,bufferData, sizeOfDataBuffer);
        pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");

        for (size_t i = sizeOfDataBuffer+1; i < sizeOfDataBuffer + bitsToAdd; i++)
        {
            bufferDataAfterChange[i] = 0;
        }

        if (bitsPerSample == 8)
        {
            if (channels == 1)
            {
                modifyHeader(bufferHeader, sizeOfDataBuffer + bitsToAdd, 1,1,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            }
            else if (channels == 2)
            {
                modifyHeader(bufferHeader, sizeOfDataBuffer + bitsToAdd, 2,2,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            }
        }
        else if (bitsPerSample == 16)
        {
            if (channels == 1)
            {
                modifyHeader(bufferHeader, sizeOfDataBuffer + bitsToAdd, 2,1,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            }
            else if (channels == 2)
            {
                modifyHeader(bufferHeader, sizeOfDataBuffer + bitsToAdd, 4,2,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            }
        }
        fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
        fwrite(bufferDataAfterChange,sizeOfDataBuffer+bitsToAdd,1,pOutputFile);
        fclose(pOutputFile);
        free(bufferDataAfterChange);
    }
    else if (secondsToChange < 0 )
    {
        secondsToChange+=abs(secondsToChange)*2;
        unsigned long int bitsToDelete = bitRate * secondsToChange;
        pOutputFile = fopen(strcat(outputFilename,".wav"),"wb");

        if (bitsPerSample == 8)
        {
            if (channels == 1)
            {
                modifyHeader(bufferHeader, sizeOfDataBuffer - bitsToDelete, 1,1,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            }
            else if (channels == 2)
            {
                modifyHeader(bufferHeader, sizeOfDataBuffer - bitsToDelete, 2,2,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            }
        }
        else if (bitsPerSample == 16)
        {
            if (channels == 1)
            {
                modifyHeader(bufferHeader, sizeOfDataBuffer - bitsToDelete, 2,1,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            }
            else if (channels == 2)
            {
                modifyHeader(bufferHeader, sizeOfDataBuffer - bitsToDelete, 4,2,(originalSampling*channels*(bitsPerSample/8)),originalSampling,bitsPerSample);
            }
        }
        fwrite(bufferHeader,SIZE_OF_WAV_HEADER,1,pOutputFile);
        fwrite(bufferData,sizeOfDataBuffer-bitsToDelete,1,pOutputFile);
        fclose(pOutputFile);
    }
    free(bufferData);
}