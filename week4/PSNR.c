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
	inputFile2 = fopen("AICenterY_Noise.bmp", "rb");
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
	double mse = 0, psnr;
	Y1 = (double *)calloc(sizeof(double) , (width * height));
	Y2 = (double *)calloc(sizeof(double) , (width * height));
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Y1[j * width + i] = inputImg[j * stride + 3 * i + 0];
			Y2[j * width + i] = inputImg1[j * stride + 3 * i + 0];
		}
	}
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			mse += (double)((Y2[j*width+i] - Y1[j*width + i])*(Y2[j*width + i] - Y1[j*width + i]));
		}
	}
	mse /= (width *height);
	psnr = mse != 0.0 ? 10.0* log10(255 * 255 / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lf db\n", mse, psnr);
	FILE *outputFile = fopen("Output5.bmp", "wb");
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
