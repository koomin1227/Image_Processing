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

	unsigned char* Y1, *Y2, *R;
	char	*P;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_rotate = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg_rotate = (unsigned char*)calloc(size, sizeof(unsigned char));

	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));
	Y2 = (unsigned char*)calloc((width * height), sizeof(unsigned char));
	R = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));
	P = (char*)malloc(sizeof(unsigned char) * (width * height));
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
			if (i == 0)
			{
				P[j * width + i] = (Y1[j * width + i] - 128) / 5;
				if (P[j * width + i] < -3) P[j * width + i] = -4;
				else if (P[j * width + i] > 2) P[j * width + i] = 3;
				R[j * width + i] = 128 + (P[j * width + i]) * 5;
			}
			else {
				P[j * width + i] = (Y1[j * width + i] - R[j * width + i - 1]) / 5;
				if (P[j * width + i] < -3) P[j * width + i] = -4;
				else if (P[j * width + i] > 2) P[j * width + i] = 3;
				R[j * width + i] = R[j * width + i - 1] + (P[j * width + i]) * 5;
			}
		}
	}
	char *bin = NULL;

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (P[j * width + i] < -3) bin = "000";
			else if (P[j * width + i] == -3) bin = "001";
			else if (P[j * width + i] == -2) bin = "010";
			else if (P[j * width + i] == -1) bin = "011";
			else if (P[j * width + i] == 0) bin = "100";
			else if (P[j * width + i] == 1) bin = "101";
			else if (P[j * width + i] == 2) bin = "110";
			else if (P[j * width + i] > 2) bin = "111";
			fprintf(textFile, "%s",bin);
			outputImg_rotate[j * stride + 3 * i + 0] = R[j * width + i];
			outputImg_rotate[j * stride + 3 * i + 1] = R[j * width + i];
			outputImg_rotate[j * stride + 3 * i + 2] = R[j * width + i];
		}
	}

	FILE* outputFile_rotate = fopen("output_rotate.bmp", "wb");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_rotate);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_rotate);
	fwrite(outputImg_rotate, sizeof(unsigned char), size, outputFile_rotate);

	free(inputImg);
	free(outputImg_rotate);

	free(Y1);
	free(Y2);

	fclose(inputFile1);
	fclose(textFile);
	fclose(outputFile_rotate);
	return 0;
}
