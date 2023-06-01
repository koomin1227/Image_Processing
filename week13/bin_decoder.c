#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>


int main() {

	BITMAPFILEHEADER bmpFile;
	BITMAPINFOHEADER bmpInfo;
	FILE* inputFile1 = NULL;
	FILE* textFile = NULL;
	inputFile1 = fopen("AICenterY.bmp", "rb");
	textFile = fopen("bitstream.txt", "r");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;

	unsigned char* Y1, *R;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_rotate = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg_rotate = (unsigned char*)calloc(size, sizeof(unsigned char));

	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));
	R = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));

	char tmp[4] = { 0 };
	int error = 0;

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			//fscanf(textFile, "%d ", &tmp);
			fread(tmp, 3, 1, textFile);

			if (strcmp(tmp, "000") == 0) error = -4;
			else if (strcmp(tmp, "001") == 0) error = -3;
			else if (strcmp(tmp, "010") == 0) error = -2;
			else if (strcmp(tmp, "011") == 0) error = -1;
			else if (strcmp(tmp, "100") == 0) error = 0;
			else if (strcmp(tmp, "101") == 0) error = 1;
			else if (strcmp(tmp, "110") == 0) error = 2;
			else if (strcmp(tmp, "111") == 0) error = 3;

			//Y1[j * width + i] = (unsigned char)error;
			//Y1[j * width + i] = Y1[j * width + i] * 5;
			error = error * 5;
			if (i == 0)
			{
				R[j * width + i] = 128 + error;
			}
			else {
				R[j * width + i] = R[j * width + i - 1] + error;
			}
		}
	}
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			outputImg_rotate[j * stride + 3 * i + 0] = R[j * width + i];
			outputImg_rotate[j * stride + 3 * i + 1] = R[j * width + i];
			outputImg_rotate[j * stride + 3 * i + 2] = R[j * width + i];
		}
	}

	FILE* outputFile_rotate = fopen("output_decoding.bmp", "wb");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_rotate);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_rotate);
	fwrite(outputImg_rotate, sizeof(unsigned char), size, outputFile_rotate);

	free(inputImg);
	free(outputImg_rotate);

	//free(Y1);
	//free(Y2);

	fclose(inputFile1);
	fclose(textFile);
	fclose(outputFile_rotate);
	return 0;
}
