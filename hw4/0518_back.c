#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>

void make_Ypicture()
{
	BITMAPFILEHEADER bmpFile;
	BITMAPINFOHEADER bmpInfo;
	FILE* inputFile = NULL;
	inputFile = fopen("front3.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile);

	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	double Y, Cb, Cr;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	fread(inputImg, sizeof(unsigned char), size, inputFile);

	//Original Copy
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Y = 0.299 * inputImg[j * stride + 3 * i + 2] + 0.587 * inputImg[j * stride + 3 * i + 1] + 0.114 * inputImg[j * stride + 3 * i + 0];
			outputImg[j * stride + 3 * i + 0] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			outputImg[j * stride + 3 * i + 1] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			outputImg[j * stride + 3 * i + 2] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
		}
	}

	FILE * outputFile = fopen("frontY.bmp", "wb");
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
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("frontY.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_down = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));


	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));


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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char*)calloc(size2, sizeof(unsigned char));
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


	FILE* outputFile_down = fopen("output_down.bmp", "wb");

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

void upsampling_basic()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("output_down.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_down = NULL, *outputImg_up = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));


	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));


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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char*)calloc(size2, sizeof(unsigned char));
	outputImg_up = (unsigned char*)calloc(size2, sizeof(unsigned char));
	double inter_filter[5][2] = { {1,0},{3.0 / 4,1.0 / 4},{2.0 / 4, 2.0 / 4},{1.0 / 4,3.0 / 4}, {0, 1} };
	double u, d, l, r;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (j < height - 1 && i < width - 1)
			{
				u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
				l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
								
				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
			
				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
			
				Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1)* width + (i)];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1)* width + (i)] * inter_filter[1][0] + Y1[(j + 1)* width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1)* width + (i)];
			}
			else if (j == height - 1 && i < width - 1)
			{
				//Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				//Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				//Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];
				//Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 +1) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4+1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4+1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4+1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

			}
			else if (i == width - 1 && j < height - 1)
			{

				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4)];


				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 1) * width2 + (i * 4)];


				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 2) * width2 + (i * 4)];


				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
			}
			else if (j == height - 1 && i == width - 1)
			{
				Y2[(j * 4 ) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 ) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 ) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];

				Y2[(j * 4 + 1) * width2 + (i * 4 )] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];


				//Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				//Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				//Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				//Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4+2) * width2 + (i * 4 )];
				//Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y2[(j * 4) * width2 + (i * 4 + 2)] + Y2[(j * 4+2) * width2 + (i * 4)]) / 2;
				//Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = (Y2[(j * 4) * width2 + (i * 4 + 3)] + Y2[(j * 4+2) * width2 + (i * 4)]) / 2;

				//Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4+3) * width2 + (i * 4 )];
				//Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = (Y2[(j * 4) * width2 + (i * 4 + 2)] + Y2[(j * 4 + 3) * width2 + (i * 4 )]) / 2;
				//Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = (Y2[(j * 4) * width2 + (i * 4 + 3)] + Y2[(j * 4 + 3) * width2 + (i * 4 )]) / 2;
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


	FILE* outputFile_down = fopen("output_up.bmp", "wb");

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

void upsampling_basic_edgev2()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("output_down.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_down = NULL, *outputImg_up = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));


	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));


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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char*)calloc(size2, sizeof(unsigned char));
	outputImg_up = (unsigned char*)calloc(size2, sizeof(unsigned char));
	double inter_filter[5][2] = { {1,0},{9.0 / 13,4.0 / 13},{2.0 / 4, 2.0 / 4},{4.0 / 13,9.0 / 13}, {0, 1} };
	double edge_filter[4] = { 4.0 / 16, 4.0 / 16, 4.0 / 16, 4.0 / 16,};
	double u, d, l, r;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (j < height - 1 && i < width - 1)
			{
				u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
				l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];

				Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];
			}
			else if (j == height - 1 && i < width - 1)
			{
				//Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				//Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				//Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];
				//Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)];


				//for (int m = 0; m < 4; m++)
				//{
				//	for (int l = 1; l < 4; l++)
				//	{
				//		Y2[(j * 4 + l) * width2 + (i * 4 + m)] = 0;
				//		for (int k = 1; k < 5; k++)
				//		{
				//			Y2[(j * 4 + l) * width2 + (i * 4 + m)] += edge_filter[k-1] * Y2[(j * 4 + l - k) * width2 + (i * 4 + m)];
				//		}
				//	}
				//}



				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

			}
			else if (i == width - 1 && j < height - 1)
			{

				for (int m = 0; m < 4; m++)
				{
					for (int l = 1; l < 4; l++)
					{
						Y2[(j * 4 + m) * width2 + (i * 4 + l)] = 0;
						for (int k = 1; k < 5; k++)
						{
							Y2[(j * 4 + m) * width2 + (i * 4 + l)] += edge_filter[k - 1] * Y2[(j * 4 + m) * width2 + (i * 4 + l - k)];
						}
					}
				}

				//Y2[(j * 4) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4)];
				//Y2[(j * 4) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4)];
				//Y2[(j * 4) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4)];


				//Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				//Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				//Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 1) * width2 + (i * 4)];


				//Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				//Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				//Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 2) * width2 + (i * 4)];


				//Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				//Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				//Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
			}
			else if (j == height - 1 && i == width - 1)
			{
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];


				//Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				//Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				//Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				//Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4+2) * width2 + (i * 4 )];
				//Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y2[(j * 4) * width2 + (i * 4 + 2)] + Y2[(j * 4+2) * width2 + (i * 4)]) / 2;
				//Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = (Y2[(j * 4) * width2 + (i * 4 + 3)] + Y2[(j * 4+2) * width2 + (i * 4)]) / 2;

				//Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4+3) * width2 + (i * 4 )];
				//Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = (Y2[(j * 4) * width2 + (i * 4 + 2)] + Y2[(j * 4 + 3) * width2 + (i * 4 )]) / 2;
				//Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = (Y2[(j * 4) * width2 + (i * 4 + 3)] + Y2[(j * 4 + 3) * width2 + (i * 4 )]) / 2;
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


	FILE* outputFile_down = fopen("output_up.bmp", "wb");

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

void upsampling_basic2()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("output_down.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_down = NULL, *outputImg_up = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));


	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));


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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char*)calloc(size2, sizeof(unsigned char));
	outputImg_up = (unsigned char*)calloc(size2, sizeof(unsigned char));
	double inter_filter[5][2] = { {1,0},{10.0 / 13,3.0 / 13},{2.0 / 4, 2.0 / 4},{3.0 / 13,10.0 / 13}, {0, 1} };
	double u, d, l, r;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (j < height - 1 && i < width - 1)
			{
				u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
				l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];

				Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];
			}
			else if (j == height - 1 && i < width - 1)
			{
				//Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				//Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				//Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];
				//Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

			}
			else if (i == width - 1 && j < height - 1)
			{

				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4)];


				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 1) * width2 + (i * 4)];


				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 2) * width2 + (i * 4)];


				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
			}
			else if (j == height - 1 && i == width - 1)
			{
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];


				//Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				//Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				//Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				//Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4+2) * width2 + (i * 4 )];
				//Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y2[(j * 4) * width2 + (i * 4 + 2)] + Y2[(j * 4+2) * width2 + (i * 4)]) / 2;
				//Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = (Y2[(j * 4) * width2 + (i * 4 + 3)] + Y2[(j * 4+2) * width2 + (i * 4)]) / 2;

				//Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4+3) * width2 + (i * 4 )];
				//Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = (Y2[(j * 4) * width2 + (i * 4 + 2)] + Y2[(j * 4 + 3) * width2 + (i * 4 )]) / 2;
				//Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = (Y2[(j * 4) * width2 + (i * 4 + 3)] + Y2[(j * 4 + 3) * width2 + (i * 4 )]) / 2;
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


	FILE* outputFile_down = fopen("output_up.bmp", "wb");

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

void upsampling_basic_asp()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("output_down.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_down = NULL, *outputImg_up = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));


	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));


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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char*)calloc(size2, sizeof(unsigned char));
	outputImg_up = (unsigned char*)calloc(size2, sizeof(unsigned char));
	double inter_filter[5][2] = { {1,0},{3.0 / 4,1.0 / 4},{2.0 / 4, 2.0 / 4},{1.0 / 4,3.0 / 4}, {0, 1} };
	double inter_filter2[5][2] = { {1,0},{37.0 / 30,-7.0 / 30},{2.0 / 4, 2.0 / 4},{-5.0 / 30,35.0 / 30}, {0, 1} };
	double inter_filter3[5][2] = { {1,0},{-7.0 / 30,37.0 / 30},{2.0 / 4, 2.0 / 4},{-5.0 / 30,35.0 / 30}, {0, 1} };
	double u, d, l, r;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (j < height - 1 && i < width - 1)
			{
				if (j < 18 && Y1[j * width + 3 * i + 0] > 60 && Y1[j * width + 3 * i + 0] < 120)
				{
					u = Y1[(j)* width + (i)] * inter_filter2[2][0] + Y1[(j)* width + (i + 1)] * inter_filter2[2][1];
					d = Y1[(j + 1) * width + (i)] * inter_filter3[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter3[2][1];
					l = Y1[(j)* width + (i)] * inter_filter2[2][0] + Y1[(j + 1) * width + (i)] * inter_filter2[2][1];
					r = Y1[(j)* width + (i + 1)] * inter_filter2[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter2[2][1];

					Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter3[1][0] + Y1[(j)* width + (i + 1)] * inter_filter3[1][1];
					Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter3[2][0] + Y1[(j)* width + (i + 1)] * inter_filter3[2][1];
					Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter2[3][0] + Y1[(j)* width + (i + 1)] * inter_filter2[3][1];
					Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

					Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter2[1][0] + Y1[(j + 1) * width + (i)] * inter_filter2[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter2[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter2[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter3[1][0] + d * inter_filter3[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter3[1][0] + Y1[(j + 1) * width + (i)] * inter_filter3[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter2[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter2[1][1];

					Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter3[2][0] + Y1[(j + 1) * width + (i)] * inter_filter3[2][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter2[1][0] + r * inter_filter2[1][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
					Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter3[3][0] + r * inter_filter3[3][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter2[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter2[2][1];

					Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter2[3][0] + Y1[(j + 1) * width + (i)] * inter_filter2[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter3[3][0] + Y1[(j + 1) * width + (i)] * inter_filter3[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter2[3][0] + d * inter_filter2[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter2[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter2[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter3[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter3[3][1];

					Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter3[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter3[1][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter2[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter2[2][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter2[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter2[3][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];
				}
				else
				{
					u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
					d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
					l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
					r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

					Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
					Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

					Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];

					Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
					Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

					Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];

					Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];
				}
			}
			else if (j == height - 1 && i < width - 1)
			{
				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];
			}
			else if (i == width - 1 && j < height - 1)
			{

				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4)];


				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 1) * width2 + (i * 4)];


				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 2) * width2 + (i * 4)];


				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
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


	FILE* outputFile_down = fopen("output_up.bmp", "wb");

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

void upsampling_basic_v2()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("output_down.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_down = NULL, *outputImg_up = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));


	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));


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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char*)calloc(size2, sizeof(unsigned char));
	outputImg_up = (unsigned char*)calloc(size2, sizeof(unsigned char));
	double inter_filter[5][2] = { {1,0},{2.0 / 4,2.0 / 4},{2.0 / 4, 2.0 / 4},{2.0 / 4,2.0 / 4}, {0, 1} };
	double u, d, l, r;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (j == 100)
				printf("d");
			if (j < height - 1 && i < width - 1)
			{
				u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
				l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];

				Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];
			}
			else if (j == height - 1 && i < width - 1)
			{
				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];
			}
			else if (i == width - 1 && j < height - 1)
			{

				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4)];


				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 1) * width2 + (i * 4)];


				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 2) * width2 + (i * 4)];


				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
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


	FILE* outputFile_down = fopen("output_up.bmp", "wb");

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

void upsampling_basic_v3()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("output_down.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_down = NULL, *outputImg_up = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));


	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));


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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char*)calloc(size2, sizeof(unsigned char));
	outputImg_up = (unsigned char*)calloc(size2, sizeof(unsigned char));
	double inter_filter[5][2] = { {1,0},{3.0 / 4,1.0 / 4},{2.0 / 4, 2.0 / 4},{1.0 / 4,3.0 / 4}, {0, 1} };
	double n_tap[5] = {-7.0/16, 15.0/16, 0, 15.0 / 16 , -7.0 / 16 };
	double tot;
	double u, d, l, r;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{

			if (j < height - 1 && i < width - 1)
			{
				u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
				l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = 0;
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = 0;
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = 0;

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];

				Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = 0;
				Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];

				tot = 0;
				for (int k = 0; k < 5; k++)
					tot += n_tap[k] * Y2[(j * 4) * width2 + (i * 4 + k)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = tot;

				tot = 0;
				for (int k = 0; k < 5; k++)
					tot += n_tap[k] * Y2[(j * 4 + 4) * width2 + (i * 4 + k)];
				Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = tot;

				tot = 0;
				for (int k = 0; k < 5; k++)
					tot += n_tap[k] * Y2[(j * 4 + k) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4)]  = tot;

				tot = 0;
				for (int k = 0; k < 5; k++)
					tot += n_tap[k] * Y2[(j * 4 + k) * width2 + (i * 4 + 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = tot;

			}
			else if (j == height - 1 && i < width - 1)
			{
				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];
			}
			else if (i == width - 1 && j < height - 1)
			{

				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4)];


				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 1) * width2 + (i * 4)];


				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 2) * width2 + (i * 4)];


				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
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


	FILE* outputFile_down = fopen("output_up.bmp", "wb");

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

void upsampling_thresh_150()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("output_down.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_down = NULL, *outputImg_up = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));


	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));


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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char*)calloc(size2, sizeof(unsigned char));
	outputImg_up = (unsigned char*)calloc(size2, sizeof(unsigned char));
	double inter_filter[5][2] = { {1,0},{3.0 / 4,1.0 / 4},{2.0 / 4, 2.0 / 4},{1.0 / 4,3.0 / 4}, {0, 1} };
	//double Ntap[5] = {};
	double u, d, l, r;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (j < height - 1 && i < width - 1)
			{
				if (Y1[(j)* width + (i)] > 250)
				{
					Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];


					Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];


					Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];


					Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)];
				}
				else {
					u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
					d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
					l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
					r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

					Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
					Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

					Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];

					Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
					Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

					Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];

					Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];
				}
			}
			else if (j == height - 1 && i < width - 1)
			{
				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];
			}
			else if (i == width - 1 && j < height - 1)
			{

				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4)];


				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 1) * width2 + (i * 4)];


				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 2) * width2 + (i * 4)];


				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
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


	FILE* outputFile_down = fopen("output_up.bmp", "wb");

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

void upsampling_thresh_v2()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("output_down.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_down = NULL, *outputImg_up = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));


	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));


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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width2, stride2, height2, size2, bitCnt);
	outputImg_down = (unsigned char*)calloc(size2, sizeof(unsigned char));
	outputImg_up = (unsigned char*)calloc(size2, sizeof(unsigned char));
	double inter_filter[5][2] = { {1,0},{3.0 / 4,1.0 / 4},{2.0 / 4, 2.0 / 4},{1.0 / 4,3.0 / 4}, {0, 1} };
	//double Ntap[5] = {};
	double u, d, l, r;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (j < height - 1 && i < width - 1)
			{
				if (Y1[(j)* width + (i)] > 100)
				{
					u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
					d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
					l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
					r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

					Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

					Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];

					Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
					Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

					Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];

					Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];
				}
				else {
					u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
					d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
					l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
					r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

					Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
					Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
					Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

					Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
					Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];

					Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
					Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
					Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

					Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];

					Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];
					Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];
				}
			}
			else if (j == height - 1 && i < width - 1)
			{
				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i + 1)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4 + 1)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4 + 2)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4 + 3)];
			}
			else if (i == width - 1 && j < height - 1)
			{

				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y2[(j * 4) * width2 + (i * 4)];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y2[(j * 4) * width2 + (i * 4)];


				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 1) * width2 + (i * 4)];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 1) * width2 + (i * 4)];


				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 2) * width2 + (i * 4)];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 2) * width2 + (i * 4)];


				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y2[(j * 4 + 3) * width2 + (i * 4)];
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


	FILE* outputFile_down = fopen("output_up.bmp", "wb");

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

void threshHolding()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL, * inputFile2 = NULL;
	inputFile1 = fopen("frontY.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);


	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	double* Y1, Cb, Cr, R, G, B, *Y2;
	double diff_Y;

	unsigned char I;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg = NULL, *outputImg2 = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg2 = (unsigned char*)calloc(size, sizeof(unsigned char));
	fread(inputImg, sizeof(unsigned char), size, inputFile1);


	//Original Copy
	int hist[256] = { 0 };
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			hist[inputImg[j * stride + 3 * i + 0]] += 1;
			if (inputImg[j * stride + 3 * i + 0] > 60 && inputImg[j * stride + 3 * i + 0] < 120 && j < 100)
			{
				outputImg2[j * stride + 3 * i + 0] = 255;
				outputImg2[j * stride + 3 * i + 1] = 255;
				outputImg2[j * stride + 3 * i + 2] = 255;
			}
			else
			{
				outputImg2[j * stride + 3 * i + 0] = 0;
				outputImg2[j * stride + 3 * i + 1] = 0;
				outputImg2[j * stride + 3 * i + 2] = 0;
			}
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
	FILE* outputFile = fopen("Output12.bmp", "wb");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile);
	fwrite(outputImg, sizeof(unsigned char), size, outputFile);

	FILE* outputFile2 = fopen("output_thresh.bmp", "wb");
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile2);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile2);
	fwrite(outputImg2, sizeof(unsigned char), size, outputFile2);

	free(outputImg);
	free(outputImg2);
	free(inputImg);
	fclose(inputFile1);
	fclose(outputFile);
	fclose(outputFile2);
}

void psnr()
{
	BITMAPFILEHEADER bmpFile, bmpFile1;
	BITMAPINFOHEADER bmpInfo, bmpInfo1;
	FILE* inputFile1 = NULL, * inputFile2 = NULL;
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
	double* Y1, Cb, Cr, R, G, B, *Y2;
	double diff_Y;

	unsigned char I;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg = NULL, *inputImg1 = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	inputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	fread(inputImg1, sizeof(unsigned char), size, inputFile2);

	//Original Copy
	double mse = 0, psnr;
	Y1 = (double*)calloc(sizeof(double), (width * height));
	Y2 = (double*)calloc(sizeof(double), (width * height));
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
			mse += (double)((Y2[j * width + i] - Y1[j * width + i]) * (Y2[j * width + i] - Y1[j * width + i]));
		}
	}
	mse /= (width * height);
	psnr = mse != 0.0 ? 10.0 * log10((255 * 255) / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lf db\n", mse, psnr);

	free(outputImg);
	free(inputImg);
	fclose(inputFile1);
	fclose(inputFile2);
}

int main() {
	//make_Ypicture();
	downSampling();
	upsampling_basic_edgev2();
	psnr();
	//threshHolding();
	return 0;
}
