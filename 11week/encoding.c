#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>


int main() {

	BITMAPFILEHEADER bmpFile;
	BITMAPINFOHEADER bmpInfo;
	FILE* inputFile1 = NULL;
	FILE* textFile = NULL;
	inputFile1 = fopen("AICenterY.bmp", "rb");
	textFile = fopen("bitstream.txt", "w");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;

	unsigned char* Y1, *Y2;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_rotate = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg_rotate = (unsigned char*)calloc(size, sizeof(unsigned char));

	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));
	Y2 = (unsigned char*)calloc((width * height), sizeof(unsigned char));
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Y1[j * width + i] = inputImg[j * stride + 3 * i + 0];
		}
	}

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			fprintf(textFile, "%d ", Y1[j * width + i] / 16);
		}
	}

	free(inputImg);
	free(outputImg_rotate);

	free(Y1);
	free(Y2);

	fclose(inputFile1);
	fclose(textFile);
	return 0;
}
