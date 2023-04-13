#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>


int main() {

	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE *inputFile1 = NULL;
	inputFile1 = fopen("AICenterY.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char *Y1,*Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char *inputImg = NULL, *outputImg_down = NULL;
	inputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	

	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char *)malloc(sizeof(unsigned char)*(width * height));
	

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Y1[j * width + i] = inputImg[j * stride + 3 * i + 0];
		}
	}
	int ratio = 2;
	int width2 = bmpInfo.biWidth >> ratio;
	int height2 = bmpInfo.biHeight >> ratio;
	int stride2 = (((bitCnt / 8) * width2) + 3) / 4 * 4;
	//int size2 = stride2 * height2;
	int size2 = stride2 * height2;
	Y2 = (unsigned char *)malloc(sizeof(unsigned char)*(width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char *)calloc(size2, sizeof(unsigned char));
	for (int j = 0; j < height2; j++)
	{
		for (int i = 0; i < width2; i++)
		{
			//subsampling
			Y2[j * width2 + i] = Y1[(j << ratio) * width + (i << ratio)];
			//average downsampling
			//Y2[j * width2 + i] = (Y1[(j << ratio) * width + (i << ratio)] + Y1[((j << ratio) + 1) * width + (i << ratio)] + Y1[(j << ratio) * width + ((i << ratio) + 1)] + Y1[((j << ratio) + 1) * width + ((i << ratio) + 1)]) / 4;
		}
	}

	for (int j = 0; j < height2; j++)
	{
		for (int i = 0; i < width2; i++)
		{
			outputImg_down[j * stride2 + 3 * i + 0] = (unsigned char)Y2[j * width2 + i];
			outputImg_down[j * stride2 + 3 * i + 1] = (unsigned char)Y2[j * width2 + i];
			outputImg_down[j * stride2 + 3 * i + 2] = (unsigned char)Y2[j * width2 + i];
		}
	}


	FILE *outputFile_down = fopen("output_down.bmp", "wb");

	bmpInfo.biWidth = width2;
	bmpInfo.biHeight = height2;
	bmpInfo.biSizeImage = size2;
	bmpFile.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size2;

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_down);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_down);
	fwrite(outputImg_down, sizeof(unsigned char), size2, outputFile_down);





	free(inputImg);
	free(outputImg_down);

	free(Y1);
	free(Y2);

	fclose(inputFile1);
	fclose(outputFile_down);

	return 0;
}
