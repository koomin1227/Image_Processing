#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>


int main() {

	BITMAPFILEHEADER bmpFile;
	BITMAPINFOHEADER bmpInfo;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("AICenterY.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;

	unsigned char* Y1, * Y2;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, * outputImg_rotate = NULL;
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
	double Gx, Gy, G;
	for (int j = 1; j < height - 1; j++)
	{
		for (int i = 1; i < width - 1; i++)
		{
			Gx = abs(Y1[j * width + i - 1] - Y1[j * width + i + 1]) / 2;
			Gy = abs(Y1[(j - 1) * width + i] - Y1[(j + 1) * width + i]) / 2;
			G = sqrt(Gx * Gx + Gy * Gy);

			//Y2[j * width + i] = (unsigned char)(Gx > 255 ? 255 : (Gx < 0 ? 0 : Gx));//x축 기준 경계 판단
			Y2[j * width + i] = (unsigned char)(Gy > 255 ? 255 : (Gy < 0 ? 0 : Gy));//y축 기준 경계 판단
			//Y2[j * width + i] = (unsigned char)(G > 50 ? 255 : (G < 0 ? 0 : G));//경계 판단
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

	FILE* outputFile_rotate = fopen("output_rotate.bmp", "wb");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_rotate);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_rotate);
	fwrite(outputImg_rotate, sizeof(unsigned char), size, outputFile_rotate);





	free(inputImg);
	free(outputImg_rotate);

	free(Y1);
	free(Y2);

	fclose(inputFile1);
	fclose(outputFile_rotate);

	return 0;
}
