#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>


int main() {

	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE *inputFile1 = NULL, *inputFile2 = NULL;
	inputFile1 = fopen("AICenterY.bmp", "rb");
	inputFile2 = fopen("AICenterY_CombinedNoise.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile2);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile2);

	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	double *Y1, Cb, Cr, R, G, B, *Y2;
	double diff_Y;

	unsigned char I;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char *inputImg = NULL, *outputImg = NULL, *inputImg1 = NULL;
	inputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	inputImg1 = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	fread(inputImg1, sizeof(unsigned char), size, inputFile2);

	//Original Copy
	int hist[256] = { 0 };
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			hist[inputImg[j * stride + 3 * i + 0]] += 1;
			outputImg[j * stride + 3 * i + 0] = 255;
			outputImg[j * stride + 3 * i + 1] = 255;
			outputImg[j * stride + 3 * i + 2] = 255;
		}
	}
	for (int k = 0; k < 256; k++)
	{
		for (int j = 0; j < hist[k] / 10; j++)
		{
			outputImg[j * stride + 3 * (k * 2) + 0] = 0;
			outputImg[j * stride + 3 * (k * 2) + 1] = 0;
			outputImg[j * stride + 3 * (k * 2) + 2] = 0;
			outputImg[j * stride + 3 * ((k * 2) + 1) + 0] = 0;
			outputImg[j * stride + 3 * ((k * 2) + 1) + 1] = 0;
			outputImg[j * stride + 3 * ((k * 2) + 1) + 2] = 0;
		}
	}
	for (int i = 0; i < 256; i++)
	{
		printf("%d : %d\n", i, hist[i]);
	}
	FILE *outputFile = fopen("Output12.bmp", "wb");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile);
	fwrite(outputImg, sizeof(unsigned char), size, outputFile);

	free(outputImg);
	free(inputImg);
	fclose(inputFile1);
	fclose(inputFile2);
	fclose(outputFile);
	return 0;
}
