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

	unsigned char* Y1, *Y2, *Y3;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char* inputImg = NULL, *outputImg_rotate = NULL;
	inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg_rotate = (unsigned char*)calloc(size, sizeof(unsigned char));

	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	Y1 = (unsigned char*)malloc(sizeof(unsigned char) * (width * height));
	Y2 = (unsigned char*)calloc((width * height), sizeof(unsigned char));
	Y3 = (unsigned char*)calloc((width * height), sizeof(unsigned char));
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (inputImg[j * stride + 3 * i + 0] > 170)
				Y1[j * width + i] = 255;
			else
				Y1[j * width + i] = 0;
		}
	}

	////erosion
	//int cnt = 0;

	//for (int j = 1; j < height - 1; j++)
	//{
	//	for (int i = 1; i < width - 1; i++)
	//	{
	//		cnt = 0;
	//		for (int k = -1; k < 2; k++)
	//		{
	//			for (int l = -1; l < 2; l++)
	//			{
	//				if (Y1[(j + k) * width + (i + l)] == 255)
	//					cnt += 1;
	//			}
	//		}
	//		if (cnt == 9)
	//			Y2[j * width + i] = 255;
	//		else
	//			Y2[j * width + i] = 0;	
	//	}
	//}

	////daliation
	//int cnt = 0;

	//for (int j = 1; j < height - 1; j++)
	//{
	//	for (int i = 1; i < width - 1; i++)
	//	{
	//		cnt = 0;
	//		for (int k = -1; k < 2; k++)
	//		{
	//			for (int l = -1; l < 2; l++)
	//			{
	//				if (Y1[(j + k) * width + (i + l)] == 255)
	//					cnt += 1;
	//			}
	//		}
	//		if (cnt == 0)
	//			Y2[j * width + i] = 0;
	//		else
	//			Y2[j * width + i] = 255;
	//	}
	//}

	//opening
	//int cnt = 0;

	//for (int j = 1; j < height - 1; j++)
	//{
	//	for (int i = 1; i < width - 1; i++)
	//	{
	//		cnt = 0;
	//		for (int k = -1; k < 2; k++)
	//		{
	//			for (int l = -1; l < 2; l++)
	//			{
	//				if (Y1[(j + k) * width + (i + l)] == 255)
	//					cnt += 1;
	//			}
	//		}
	//		if (cnt == 9)
	//			Y2[j * width + i] = 255;
	//		else
	//			Y2[j * width + i] = 0;
	//	}
	//}

	//for (int j = 1; j < height - 1; j++)
	//{
	//	for (int i = 1; i < width - 1; i++)
	//	{
	//		cnt = 0;
	//		for (int k = -1; k < 2; k++)
	//		{
	//			for (int l = -1; l < 2; l++)
	//			{
	//				if (Y2[(j + k) * width + (i + l)] == 255)
	//					cnt += 1;
	//			}
	//		}
	//		if (cnt == 0)
	//			Y3[j * width + i] = 0;
	//		else
	//			Y3[j * width + i] = 255;
	//	}
	//}

	//closing
	int cnt = 0;

	for (int j = 1; j < height - 1; j++)
	{
		for (int i = 1; i < width - 1; i++)
		{
			cnt = 0;
			for (int k = -1; k < 2; k++)
			{
				for (int l = -1; l < 2; l++)
				{
					if (Y1[(j + k) * width + (i + l)] == 255)
						cnt += 1;
				}
			}
			if (cnt == 0)
				Y3[j * width + i] = 0;
			else
				Y3[j * width + i] = 255;
		}
	}

	for (int j = 1; j < height - 1; j++)
	{
		for (int i = 1; i < width - 1; i++)
		{
			cnt = 0;
			for (int k = -1; k < 2; k++)
			{
				for (int l = -1; l < 2; l++)
				{
					if (Y3[(j + k) * width + (i + l)] == 255)
						cnt += 1;
				}
			}
			if (cnt == 9)
				Y2[j * width + i] = 255;
			else
				Y2[j * width + i] = 0;
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

	FILE* outputFile_rotate = fopen("output_thresh_closing.bmp", "wb");
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
