#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

void ReadImage(const char* fileName, const char* outfileName, byte** pixels, int32* width, int32* height, int32* bytesPerPixel, FILE*& imageFile, FILE*& OUT)
{

    imageFile = fopen(fileName, "rb");
    int32 dataOffset;
    int32 LookUpTable=0; 
    fseek(imageFile, HEADER_SIZE + INFO_HEADER_SIZE-8, SEEK_SET);
    fread(&LookUpTable, 4, 1, imageFile);
    fseek(imageFile, 0, SEEK_SET);

    OUT = fopen(outfileName, "wb");

    int header = 0;
    if (LookUpTable)
        header = HEADER_SIZE + INFO_HEADER_SIZE + 1024;
    else
        header = HEADER_SIZE + INFO_HEADER_SIZE;
    for (int i = 0; i < header; i++)
    {
        int get = getc(imageFile);
        putc(get, OUT);
    }

    fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
    fread(&dataOffset, 4, 1, imageFile);
    fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
    fread(width, 4, 1, imageFile);
    fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
    fread(height, 4, 1, imageFile);
    int16 bitsPerPixel;
    fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
    fread(&bitsPerPixel, 2, 1, imageFile);
    *bytesPerPixel = ((int32)bitsPerPixel) / 8; //3 bytes per pixel when color, 1 byte per pixel when grayscale

    int paddedRowSize = (int)(4 * (float)(*width) / 4.0f) * (*bytesPerPixel);
    int unpaddedRowSize = (*width) * (*bytesPerPixel);
    int totalSize = unpaddedRowSize * (*height);

    *pixels = new byte[totalSize];
    int i = 0;
    byte* currentRowPointer = *pixels + ((*height - 1) * unpaddedRowSize);
    for (i = 0; i < *height; i++)
    {
        fseek(imageFile, dataOffset + (i * paddedRowSize), SEEK_SET);
        fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
        currentRowPointer -= unpaddedRowSize;
    }
    fclose(imageFile);
}

void WriteImage(byte* pixels, int32 width, int32 height, int32 bytesPerPixel, FILE*& outputFile, char mode)
{
    int paddedRowSize = (int)(4 * (float)width / 4.0f) * bytesPerPixel;
    int unpaddedRowSize = width * bytesPerPixel;
    if(mode == 'B')
    { // Extract blue color
        for (int i = 0; i < height; i++)
        {
            int pixelOffset = ((height - i) - 1) * unpaddedRowSize;
            // Iterate 1/3 times of #col 
            // stride = 3*i
            for(int i=0;i<paddedRowSize/3;i++) {
                int stride = 3*i;
                fwrite(&pixels[pixelOffset+stride], 1, 1, outputFile); // B
                fwrite("0", 1, 1, outputFile); // G
                fwrite("0", 1, 1, outputFile); // R
            }
        }
    }
    else if(mode == 'G')
    { // Extract green color
        for (int i = 0; i < height; i++)
        {
            int pixelOffset = ((height - i) - 1) * unpaddedRowSize;
            // Iterate 1/3 times of #col 
            // stride = 3*i
            for(int i=0;i<paddedRowSize/3;i++) {
                int stride = 3*i;
                fwrite("0", 1, 1, outputFile); // B
                fwrite(&pixels[pixelOffset+stride+1], 1, 1, outputFile); // G
                fwrite("0", 1, 1, outputFile); // R
            }
        }
    }
    else if(mode == 'R')
    { // Extract red color
        for (int i = 0; i < height; i++)
        {
            int pixelOffset = ((height - i) - 1) * unpaddedRowSize;
            // Iterate 1/3 times of #col 
            // stride = 3*i
            for(int i=0;i<paddedRowSize/3;i++) {
                int stride = 3*i;
                fwrite("0", 1, 1, outputFile); // B
                fwrite("0", 1, 1, outputFile); // G
                fwrite(&pixels[pixelOffset+stride+2], 1, 1, outputFile); // R
            }
        }
    }
    else
    { // default: gray mode
        for (int i = 0; i < height; i++)
        {
            int pixelOffset = ((height - i) - 1) * unpaddedRowSize;
            // Iterate 1/3 times of #col 
            // stride = 3*i
            for(int i=0;i<paddedRowSize/3;i++) {
                int stride = 3*i;
                fwrite(&pixels[pixelOffset+stride], 1, 1, outputFile); // B
                fwrite(&pixels[pixelOffset+stride], 1, 1, outputFile); // G
                fwrite(&pixels[pixelOffset+stride], 1, 1, outputFile); // R
            }
        }
    }
    fclose(outputFile);
}

int main()
{
    byte* pixels;
    int32 width;
    int32 height;
    int32 bytesPerPixel;
    FILE* imageFile;
    FILE* outputFile;
    ReadImage("Lion.bmp", "./bmprw/Lion_B.bmp", &pixels, &width, &height, &bytesPerPixel, imageFile, outputFile);
    WriteImage(pixels, width, height, bytesPerPixel, outputFile, 'B'); // Extract blue color
    ReadImage("Lion.bmp", "./bmprw/Lion_G.bmp", &pixels, &width, &height, &bytesPerPixel, imageFile, outputFile);
    WriteImage(pixels, width, height, bytesPerPixel, outputFile, 'G'); // Extract green color
    ReadImage("Lion.bmp", "./bmprw/Lion_R.bmp", &pixels, &width, &height, &bytesPerPixel, imageFile, outputFile);
    WriteImage(pixels, width, height, bytesPerPixel, outputFile, 'R'); // Extract red color
    ReadImage("Lion.bmp", "./bmprw/Lion_.bmp", &pixels, &width, &height, &bytesPerPixel, imageFile, outputFile);
    WriteImage(pixels, width, height, bytesPerPixel, outputFile,NULL); // Convert to gray iamge
    delete[] pixels;
    return 0;
}