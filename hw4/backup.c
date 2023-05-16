#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>

void make_Ypicture()
{
	BITMAPFILEHEADER bmpFile;
	BITMAPINFOHEADER bmpInfo;
	FILE *inputFile = NULL;
	inputFile = fopen("front.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile);

	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	double Y, Cb, Cr;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char *inputImg = NULL, *outputImg = NULL;
	inputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	fread(inputImg, sizeof(unsigned char), size, inputFile);

	//Original Copy
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Y = 0.299  * inputImg[j * stride + 3 * i + 2] + 0.587 * inputImg[j * stride + 3 * i + 1] + 0.114 * inputImg[j * stride + 3 * i + 0];
			outputImg[j * stride + 3 * i + 0] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			outputImg[j * stride + 3 * i + 1] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			outputImg[j * stride + 3 * i + 2] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
		}
	}

	FILE *outputFile = fopen("frontY.bmp", "wb");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile);
	fwrite(outputImg, sizeof(unsigned char), size, outputFile);

	free(outputImg);
	free(inputImg);
	fclose(inputFile);
	fclose(outputFile);
	return 0;

}

void downSampling()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE *inputFile1 = NULL;
	inputFile1 = fopen("frontY.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char *Y1, *Y2;

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

}

void upsampling()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE *inputFile1 = NULL;
	inputFile1 = fopen("output_down.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char *Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char *inputImg = NULL, *outputImg_down = NULL, *outputImg_up = NULL;
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
	int width2 = bmpInfo.biWidth << ratio;
	int height2 = bmpInfo.biHeight << ratio;
	int stride2 = (((bitCnt / 8) * width2) + 3) / 4 * 4;
	int size2 = stride2 * height2;
	int nj, ni;
	Y2 = (unsigned char *)malloc(sizeof(unsigned char)*(width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char *)calloc(size2, sizeof(unsigned char));
	outputImg_up = (unsigned char *)calloc(size2, sizeof(unsigned char));
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (j == 100)
				printf("d");
			if (j < height - 1 && i < width - 1)
			{
				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j) * width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)])/2;
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4+1) * width2 + (i * 4)] = (Y1[(j)* width + (i)] + Y1[(j + 1)* width + (i)])/2;
				Y2[(j * 4+1) * width2 + (i * 4 + 1)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1)* width + (i)] + Y1[(j + 1)* width + (i + 1)])/4;
				Y2[(j * 4+1) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i + 1)] + Y1[(j + 1)* width + (i + 1)]) / 2;

				Y2[(j * 4+2) * width2 + (i * 4)] = Y1[(j + 1)* width + (i)];
				Y2[(j * 4+2) * width2 + (i * 4 + 1)] = (Y1[(j + 1)* width + (i)] + Y1[(j + 1)* width + (i + 1)])/2;
				Y2[(j * 4+2) * width2 + (i * 4 + 2)] = Y1[(j + 1)* width + (i + 1)];
			}
			else if (j == height - 1 && i < width - 1)
			{
				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)]) / 2;
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];
			}
			else if (i == width - 1 && j < height - 1)
			{
				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4)] = (Y1[(j)* width + (i)] + Y1[(j + 1)* width + (i)]) / 2;
				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j + 1)* width + (i)];
			}
			else if (j == height - 1 && i == width - 1)
			{
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
			}
			//Y2[j * width2 + i] = Y1[(j >> ratio) * width + (i >> ratio)];
		}
	}


	for (int j = 0; j < height2; j++)
	{
		for (int i = 0; i < width2; i++)
		{
			outputImg_up[j * stride2 + 3 * i + 0] = (unsigned char)Y2[j * width2 + i];
			outputImg_up[j * stride2 + 3 * i + 1] = (unsigned char)Y2[j * width2 + i];
			outputImg_up[j * stride2 + 3 * i + 2] = (unsigned char)Y2[j * width2 + i];
		}
	}


	FILE *outputFile_down = fopen("output_up.bmp", "wb");

	bmpInfo.biWidth = width2;
	bmpInfo.biHeight = height2;
	bmpInfo.biSizeImage = size2;
	bmpFile.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size2;
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_down);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_down);
	fwrite(outputImg_up, sizeof(unsigned char), size2, outputFile_down);

	free(inputImg);
	free(outputImg_down);

	free(Y1);
	free(Y2);

	fclose(inputFile1);
	fclose(outputFile_down);
}

void psnr()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE *inputFile1 = NULL, *inputFile2 = NULL;
	inputFile1 = fopen("frontY.bmp", "rb");
	inputFile2 = fopen("output_up.bmp", "rb");
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
	Y1 = (double *)calloc(sizeof(double), (width * height));
	Y2 = (double *)calloc(sizeof(double), (width * height));
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
			mse += (double)((Y2[j*width + i] - Y1[j*width + i])*(Y2[j*width + i] - Y1[j*width + i]));
		}
	}
	mse /= (width *height);
	psnr = mse != 0.0 ? 10.0* log10((255 * 255) / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lf db\n", mse, psnr);

	free(outputImg);
	free(inputImg);
	fclose(inputFile1);
	fclose(inputFile2);
}

int main() {
	//make_Ypicture();
	//downSampling();
	upsampling();
	//psnr();
	return 0;
}
