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
	double *Y, Cb, Cr, R, G, B, *Y2;
	double diff_Y;

	unsigned char I;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char *inputImg = NULL, *outputImg = NULL, *inputImg1 = NULL, *dark = NULL, *outputImg2 = NULL, *outputImg3 = NULL, *outputImg4 = NULL, *outputImg5 = NULL;
	inputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	inputImg1 = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg2 = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg3 = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg4 = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg5 = (unsigned char *)calloc(size, sizeof(unsigned char));

	dark = (unsigned char *)calloc(size, sizeof(unsigned char));
	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	fread(inputImg1, sizeof(unsigned char), size, inputFile2);
	Y = (double *)malloc(sizeof(double)*(width * height));
	//Original Copy
	int hist[256] = { 0 };
	int sum_hist[256] = { 0 };
	int hist_he[256] = { 0 };
	int sum_hist_he[256] = { 0 };
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			dark[j * stride + 3 * i + 0] = inputImg[j * stride + 3 * i + 0] / 8;
			dark[j * stride + 3 * i + 1] = inputImg[j * stride + 3 * i + 0] / 8;
			dark[j * stride + 3 * i + 2] = inputImg[j * stride + 3 * i + 0] / 8;

			hist[inputImg[j * stride + 3 * i + 0] / 8] += 1;
			outputImg[j * stride + 3 * i + 0] = 255;
			outputImg[j * stride + 3 * i + 1] = 255;
			outputImg[j * stride + 3 * i + 2] = 255;

			outputImg2[j * stride + 3 * i + 0] = 255;
			outputImg2[j * stride + 3 * i + 1] = 255;
			outputImg2[j * stride + 3 * i + 2] = 255;

			outputImg4[j * stride + 3 * i + 0] = 255;
			outputImg4[j * stride + 3 * i + 1] = 255;
			outputImg4[j * stride + 3 * i + 2] = 255;

			outputImg5[j * stride + 3 * i + 0] = 255;
			outputImg5[j * stride + 3 * i + 1] = 255;
			outputImg5[j * stride + 3 * i + 2] = 255;
		}
	}

	printf("check!\n");
	for (int i = 0; i < 256; i++)
	{
		printf("%d : %d\n", i, hist[i]);
	}
	sum_hist[0] = hist[0];
	for (int i = 1; i < 256; i++)
		sum_hist[i] = hist[i] + sum_hist[i - 1];

	printf("check!\n");


	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Y[j * width + i] = 255 * sum_hist[dark[j * stride + 3 * i + 0]] / (width * height);
			hist_he[(int)Y[j * width + i]] += 1;
			outputImg3[j * stride + 3 * i + 0] = Y[j * width + i];
			outputImg3[j * stride + 3 * i + 1] = Y[j * width + i];
			outputImg3[j * stride + 3 * i + 2] = Y[j * width + i];
		}
	}
	for (int i = 1; i < 256; i++)
		sum_hist_he[i] = hist_he[i] + sum_hist_he[i - 1];
	for (int k = 0; k < 256; k++)
	{
		for (int j = 0; j < hist[k] / 20; j++)
		{
			outputImg[j * stride + 3 * (k * 2) + 0] = 0;
			outputImg[j * stride + 3 * (k * 2) + 1] = 0;
			outputImg[j * stride + 3 * (k * 2) + 2] = 0;
			outputImg[j * stride + 3 * ((k * 2) + 1) + 0] = 0;
			outputImg[j * stride + 3 * ((k * 2) + 1) + 1] = 0;
			outputImg[j * stride + 3 * ((k * 2) + 1) + 2] = 0;
		}

		for (int j = 0; j < sum_hist[k] / 512; j++)
		{
			outputImg2[j * stride + 3 * (k * 2) + 0] = 0;
			outputImg2[j * stride + 3 * (k * 2) + 1] = 0;
			outputImg2[j * stride + 3 * (k * 2) + 2] = 0;
			outputImg2[j * stride + 3 * ((k * 2) + 1) + 0] = 0;
			outputImg2[j * stride + 3 * ((k * 2) + 1) + 1] = 0;
			outputImg2[j * stride + 3 * ((k * 2) + 1) + 2] = 0;
		}

		for (int j = 0; j < hist_he[k] / 20; j++)
		{
			outputImg4[j * stride + 3 * (k * 2) + 0] = 0;
			outputImg4[j * stride + 3 * (k * 2) + 1] = 0;
			outputImg4[j * stride + 3 * (k * 2) + 2] = 0;
			outputImg4[j * stride + 3 * ((k * 2) + 1) + 0] = 0;
			outputImg4[j * stride + 3 * ((k * 2) + 1) + 1] = 0;
			outputImg4[j * stride + 3 * ((k * 2) + 1) + 2] = 0;
		}

		for (int j = 0; j < sum_hist_he[k] / 512; j++)
		{
			outputImg5[j * stride + 3 * (k * 2) + 0] = 0;
			outputImg5[j * stride + 3 * (k * 2) + 1] = 0;
			outputImg5[j * stride + 3 * (k * 2) + 2] = 0;
			outputImg5[j * stride + 3 * ((k * 2) + 1) + 0] = 0;
			outputImg5[j * stride + 3 * ((k * 2) + 1) + 1] = 0;
			outputImg5[j * stride + 3 * ((k * 2) + 1) + 2] = 0;
		}
	}
	printf("check!\n");
	
	FILE *outputFile_hist = fopen("hist.bmp", "wb");
	FILE *outputFile_sum_hist = fopen("sum_hist.bmp", "wb");
	FILE *outputFile = fopen("dark_ai.bmp", "wb");
	FILE *outputFile_he = fopen("dark_ai_he.bmp", "wb");
	FILE *outputFile_hist_he = fopen("hist_he.bmp", "wb");
	FILE *outputFile_sum_hist_he = fopen("sum_hist_he.bmp", "wb");
	printf("check4\n");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_hist);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_hist);
	fwrite(outputImg, sizeof(unsigned char), size, outputFile_hist);

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_sum_hist);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_sum_hist);
	fwrite(outputImg2, sizeof(unsigned char), size, outputFile_sum_hist);

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile);
	fwrite(dark, sizeof(unsigned char), size, outputFile);

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_he);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_he);
	fwrite(outputImg3, sizeof(unsigned char), size, outputFile_he);

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_hist_he);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_hist_he);
	fwrite(outputImg4, sizeof(unsigned char), size, outputFile_hist_he);

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_sum_hist_he);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_sum_hist_he);
	fwrite(outputImg5, sizeof(unsigned char), size, outputFile_sum_hist_he);

	printf("check3\n");
	free(outputImg);
	free(inputImg);
	fclose(inputFile1);
	fclose(inputFile2);
	fclose(outputFile);
	fclose(outputFile_hist);
	fclose(outputFile_sum_hist);
	fclose(outputFile_he);
	fclose(outputFile_hist_he);
	fclose(outputFile_sum_hist_he);
	return 0;
}
