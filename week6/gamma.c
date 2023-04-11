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
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	double *Y1, Cb, Cr, R, G, B, *Y2,*Y3;
	double diff_Y;

	unsigned char I;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char *inputImg = NULL, *outputImg = NULL, *inputImg1 = NULL, *outputImg2 = NULL;
	inputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	inputImg1 = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg2 = (unsigned char *)calloc(size, sizeof(unsigned char));

	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (double *)malloc(sizeof(double)*(width * height));
	Y2 = (double *)malloc(sizeof(double)*(width * height));
	Y3 = (double *)malloc(sizeof(double)*(width * height));
	//Original Copy
	double gamma;
	gamma = 2.5;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Y1[j*width + i] = inputImg[j * stride + 3 * i + 0];
			//Y2[j*width + i] = inputImg[j * stride + 3 * i + 0];
			Y2[j*width + i] = (unsigned char)(255 * pow(Y1[j*width + i] / 255.0, 1 / gamma));
			Y3[j*width + i] = (unsigned char)(255 * pow(Y2[j*width + i] / 255.0,  gamma));
			outputImg[j * stride + 3 * i + 0] = (unsigned char)(255 * pow(Y1[j*width + i] / 255.0, 1/gamma));
			outputImg[j * stride + 3 * i + 1] = (unsigned char)(255 * pow(Y1[j*width + i] / 255.0, 1 / gamma));
			outputImg[j * stride + 3 * i + 2] = (unsigned char)(255 * pow(Y1[j*width + i] / 255.0, 1 / gamma));

			outputImg2[j * stride + 3 * i + 0] = Y3[j*width + i];
			outputImg2[j * stride + 3 * i + 1] = Y3[j*width + i];
			outputImg2[j * stride + 3 * i + 2] = Y3[j*width + i];
		}
	}
	double mse=0;
	double psnr;

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			mse += (double)((Y1[j*width + i] - Y3[j*width + i])*(Y1[j*width + i] - Y3[j*width + i]));
		}
	}
	mse /= (width *height);
	psnr = mse != 0.0 ? 10.0* log10(255 * 255 / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lf db\n", mse, psnr);

	



	FILE *outputFile = fopen("gamma_ai.bmp", "wb");
	FILE *outputFile2 = fopen("gamma_ai_to_origin.bmp", "wb");

	printf("check4\n");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile);
	fwrite(outputImg, sizeof(unsigned char), size, outputFile);

	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile2);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile2);
	fwrite(outputImg2, sizeof(unsigned char), size, outputFile2);

	free(outputImg);
	free(inputImg);
	fclose(inputFile1);
	fclose(outputFile);
	fclose(outputFile2);
	return 0;
}
