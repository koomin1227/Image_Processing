#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>


int main() {

	BITMAPFILEHEADER bmpFile;
	BITMAPINFOHEADER bmpInfo;
	FILE *inputFile1 = NULL;
	inputFile1 = fopen("AICenterY.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	double pi = 3.1415926535;
	double seta = pi / 4;
	unsigned char *Y1, *Y2;
	int x0 = 256, y0 = 256, x1, y1, x2, y2;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char *inputImg = NULL, *outputImg_rotate = NULL;
	inputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg_rotate = (unsigned char *)calloc(size, sizeof(unsigned char));

	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char *)malloc(sizeof(unsigned char)*(width * height));
	Y2 = (unsigned char *)calloc((width * height), sizeof(unsigned char));
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
			x1 = i;
			y1 = j;
			x2 = cos(seta) * (x1 - x0) - sin(seta) * (y1 - y0) + x0;
			y2 = sin(seta) * (x1 - x0) + cos(seta) * (y1 - y0) + y0;
			if (x2 >= 0 && x2 <= 512 && y2 >= 0 && y2 <= 512)
				Y2[y2 * width + x2] = Y1[j * width + i];;
		}
	}
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			outputImg_rotate[j * stride + 3 * i + 0] = Y2[j * width + i];
			outputImg_rotate[j * stride + 3 * i + 1] = Y2[j * width + i];
			outputImg_rotate[j * stride + 3 * i + 2] = Y2[j * width + i];
		}
	}

	FILE *outputFile_rotate = fopen("output_rotate.bmp", "wb");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_rotate);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_rotate);
	fwrite(outputImg_rotate, sizeof(unsigned char), size, outputFile_rotate);





	free(inputImg);
	free(outputImg_rotate);

	//free(Y1);
	//free(Y2);

	fclose(inputFile1);
	fclose(outputFile_rotate);

	return 0;
}
