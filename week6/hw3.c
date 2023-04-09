#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>


int main() {

	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE *inputFile1 = NULL;
	inputFile1 = fopen("myroom.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	double *Y;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char *inputImg = NULL, *darkImg = NULL,*origin_hist = NULL, *he_hist = NULL, *he_img = NULL;
	inputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	darkImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	he_img = (unsigned char *)calloc(size, sizeof(unsigned char));
	origin_hist = (unsigned char *)calloc(size, sizeof(unsigned char));
	he_hist = (unsigned char *)calloc(size, sizeof(unsigned char));

	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y = (double *)malloc(sizeof(double)*(width * height));

	int hist[256] = { 0 };
	int sum_hist[256] = { 0 };
	int hist_he[256] = { 0 };
	int sum_hist_he[256] = { 0 };
	double y_tmp;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			y_tmp = 0.299  * inputImg[j * stride + 3 * i + 2] + 0.587 * inputImg[j * stride + 3 * i + 1] + 0.114 * inputImg[j * stride + 3 * i + 0];

			darkImg[j * stride + 3 * i + 0] = (unsigned char)(y_tmp > 255 ? 255 : (y_tmp < 0 ? 0 : y_tmp));
			darkImg[j * stride + 3 * i + 1] = (unsigned char)(y_tmp > 255 ? 255 : (y_tmp < 0 ? 0 : y_tmp));
			darkImg[j * stride + 3 * i + 2] = (unsigned char)(y_tmp > 255 ? 255 : (y_tmp < 0 ? 0 : y_tmp));
			//darkImg[j * stride + 3 * i + 0] = 0.299  * inputImg[j * stride + 3 * i + 2] + 0.587 * inputImg[j * stride + 3 * i + 1] + 0.114 * inputImg[j * stride + 3 * i + 0];
			//darkImg[j * stride + 3 * i + 1] = 0.299  * inputImg[j * stride + 3 * i + 2] + 0.587 * inputImg[j * stride + 3 * i + 1] + 0.114 * inputImg[j * stride + 3 * i + 0];
			//darkImg[j * stride + 3 * i + 2] = 0.299  * inputImg[j * stride + 3 * i + 2] + 0.587 * inputImg[j * stride + 3 * i + 1] + 0.114 * inputImg[j * stride + 3 * i + 0];
			
			hist[inputImg[j * stride + 3 * i + 0]] += 1;

			origin_hist[j * stride + 3 * i + 0] = 255;
			origin_hist[j * stride + 3 * i + 1] = 255;
			origin_hist[j * stride + 3 * i + 2] = 255;

			he_hist[j * stride + 3 * i + 0] = 255;
			he_hist[j * stride + 3 * i + 1] = 255;
			he_hist[j * stride + 3 * i + 2] = 255;
		}
	}


	for (int i = 0; i < 256; i++)
	{
		printf("%d : %d\n", i, hist[i]);
	}

	sum_hist[0] = hist[0];
	for (int i = 1; i < 256; i++)
		sum_hist[i] = hist[i] + sum_hist[i - 1];



	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Y[j * width + i] = 255 * sum_hist[darkImg[j * stride + 3 * i + 0]] / (width * height);
			hist_he[(int)Y[j * width + i]] += 1;
			he_img[j * stride + 3 * i + 0] = Y[j * width + i];
			he_img[j * stride + 3 * i + 1] = Y[j * width + i];
			he_img[j * stride + 3 * i + 2] = Y[j * width + i];
		}
	}
	sum_hist_he[0] = hist_he[0];
	for (int i = 1; i < 256; i++)
		sum_hist_he[i] = hist_he[i] + sum_hist_he[i - 1];

	for (int k = 0; k < 256; k++)
	{
		for (int j = 0; j < hist[k] / 70; j++)
		{
			origin_hist[j * stride + 3 * (k * 2) + 0] = 0;
			origin_hist[j * stride + 3 * (k * 2) + 1] = 0;
			origin_hist[j * stride + 3 * (k * 2) + 2] = 0;
			origin_hist[j * stride + 3 * ((k * 2) + 1) + 0] = 0;
			origin_hist[j * stride + 3 * ((k * 2) + 1) + 1] = 0;
			origin_hist[j * stride + 3 * ((k * 2) + 1) + 2] = 0;
		}

		//for (int j = 0; j < sum_hist[k] / 512; j++)
		//{
		//	outputImg2[j * stride + 3 * (k * 2) + 0] = 0;
		//	outputImg2[j * stride + 3 * (k * 2) + 1] = 0;
		//	outputImg2[j * stride + 3 * (k * 2) + 2] = 0;
		//	outputImg2[j * stride + 3 * ((k * 2) + 1) + 0] = 0;
		//	outputImg2[j * stride + 3 * ((k * 2) + 1) + 1] = 0;
		//	outputImg2[j * stride + 3 * ((k * 2) + 1) + 2] = 0;
		//}

		for (int j = 0; j < hist_he[k] / 70; j++)
		{
			he_hist[j * stride + 3 * (k * 2) + 0] = 0;
			he_hist[j * stride + 3 * (k * 2) + 1] = 0;
			he_hist[j * stride + 3 * (k * 2) + 2] = 0;
			he_hist[j * stride + 3 * ((k * 2) + 1) + 0] = 0;
			he_hist[j * stride + 3 * ((k * 2) + 1) + 1] = 0;
			he_hist[j * stride + 3 * ((k * 2) + 1) + 2] = 0;
		}

		//for (int j = 0; j < sum_hist_he[k] / 512; j++)
		//{
		//	outputImg5[j * stride + 3 * (k * 2) + 0] = 0;
		//	outputImg5[j * stride + 3 * (k * 2) + 1] = 0;
		//	outputImg5[j * stride + 3 * (k * 2) + 2] = 0;
		//	outputImg5[j * stride + 3 * ((k * 2) + 1) + 0] = 0;
		//	outputImg5[j * stride + 3 * ((k * 2) + 1) + 1] = 0;
		//	outputImg5[j * stride + 3 * ((k * 2) + 1) + 2] = 0;
		//}
	}


	FILE *outputFile_hist = fopen("hist.bmp", "wb");
	//FILE *outputFile_sum_hist = fopen("sum_hist.bmp", "wb");
	FILE *outputFile = fopen("dark_ai.bmp", "wb");
	FILE *outputFile_he = fopen("dark_ai_he.bmp", "wb");
	FILE *outputFile_hist_he = fopen("hist_he.bmp", "wb");
	//FILE *outputFile_sum_hist_he = fopen("sum_hist_he.bmp", "wb");

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_hist);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_hist);
	fwrite(origin_hist, sizeof(unsigned char), size, outputFile_hist);

	//fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_sum_hist);
	//fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_sum_hist);
	//fwrite(outputImg2, sizeof(unsigned char), size, outputFile_sum_hist);

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile);
	fwrite(darkImg, sizeof(unsigned char), size, outputFile);

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_he);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_he);
	fwrite(he_img, sizeof(unsigned char), size, outputFile_he);

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_hist_he);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_hist_he);
	fwrite(he_hist, sizeof(unsigned char), size, outputFile_hist_he);

	//fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_sum_hist_he);
	//fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_sum_hist_he);
	//fwrite(outputImg5, sizeof(unsigned char), size, outputFile_sum_hist_he);



	free(inputImg);
	free(darkImg);
	free(origin_hist);
	free(he_hist);
	free(he_img);
	free(Y);

	fclose(inputFile1);
	fclose(outputFile);
	fclose(outputFile_hist);
	fclose(outputFile_he);
	fclose(outputFile_hist_he);

	//fclose(outputFile_sum_hist_he);
	//fclose(outputFile_sum_hist);
	return 0;
}
