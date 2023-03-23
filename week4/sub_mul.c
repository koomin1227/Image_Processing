#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


int main() {

	BITMAPFILEHEADER bmpFile;
	BITMAPINFOHEADER bmpInfo;
	FILE *inputFile1 = NULL, *inputFile2 = NULL;
	inputFile1 = fopen("AICenter.bmp", "rb");
	inputFile2 = fopen("binary.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	double Y, Cb, Cr, R, G, B;
	double diff_Y;

	unsigned char I;
	printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

	unsigned char *inputImg = NULL, *outputImg = NULL, *inputImg1 = NULL;
	inputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	inputImg1 = (unsigned char *)calloc(size, sizeof(unsigned char));
	//inputImg1 = (unsigned char *)calloc(size, sizeof(unsigned char));
	//inputImg2 = (unsigned char *)calloc(size, sizeof(unsigned char));
	outputImg = (unsigned char *)calloc(size, sizeof(unsigned char));
	fread(inputImg, sizeof(unsigned char), size, inputFile1);
	fread(inputImg1, sizeof(unsigned char), size, inputFile2);

	//Original Copy
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (inputImg1[j * stride + 3 * i + 2] == 0 && inputImg1[j * stride + 3 * i + 1] == 0 && inputImg1[j * stride + 3 * i + 0] == 0)
			{
				outputImg[j * stride + 3 * i + 0]=1;
				outputImg[j * stride + 3 * i + 1]=1;
				outputImg[j * stride + 3 * i + 2]=1;
				//outputImg[j * stride + 3 * i + 0] = (unsigned char)(inputImg1[j * stride + 3 * i + 0] * inputImg[j * stride + 3 * i + 0] > 255 ? 255 : (inputImg1[j * stride + 3 * i + 0] * inputImg[j * stride + 3 * i + 0] < 0 ? 0 : inputImg1[j * stride + 3 * i + 0] * inputImg[j * stride + 3 * i + 0]));
				//outputImg[j * stride + 3 * i + 1] = (unsigned char)(inputImg1[j * stride + 3 * i + 1] * inputImg[j * stride + 3 * i + 1] > 255 ? 255 : (inputImg1[j * stride + 3 * i + 1] * inputImg[j * stride + 3 * i + 1] < 0 ? 0 : inputImg1[j * stride + 3 * i + 1] * inputImg[j * stride + 3 * i + 1]));
				//outputImg[j * stride + 3 * i + 2] = (unsigned char)(inputImg1[j * stride + 3 * i + 2] * inputImg[j * stride + 3 * i + 2] > 255 ? 255 : (inputImg1[j * stride + 3 * i + 2] * inputImg[j * stride + 3 * i + 2] < 0 ? 0 : inputImg1[j * stride + 3 * i + 2] * inputImg[j * stride + 3 * i + 2]));
				
			}
			else
			{
				outputImg[j * stride + 3 * i + 0] = inputImg[j * stride + 3 * i + 0];
				outputImg[j * stride + 3 * i + 1] = inputImg[j * stride + 3 * i + 1];
				outputImg[j * stride + 3 * i + 2] = inputImg[j * stride + 3 * i + 2];
			}









			//Y = 0.299  * inputImg[j * stride + 3 * i + 2] + 0.587 * inputImg[j * stride + 3 * i + 1] + 0.114 * inputImg[j * stride + 3 * i + 0];
			//Cb = -0.169 * inputImg[j * stride + 3 * i + 2] - 0.331 * inputImg[j * stride + 3 * i + 1] + 0.500 * inputImg[j * stride + 3 * i + 0];
			//Cr = 0.500 * inputImg[j * stride + 3 * i + 2] - 0.419 * inputImg[j * stride + 3 * i + 1] - 0.0813 * inputImg[j * stride + 3 * i + 0];
			//I = (unsigned char)((inputImg[j * stride + 3 * i + 2] + inputImg[j * stride + 3 * i + 1] + inputImg[j * stride + 3 * i + 0]) / 3);
			////outputImg[j * stride + 3 * i + 0] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			////outputImg[j * stride + 3 * i + 1] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			////outputImg[j * stride + 3 * i + 2] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			////outputImg[j * stride + 3 * i + 0] = I;
			////outputImg[j * stride + 3 * i + 1] = I;
			////outputImg[j * stride + 3 * i + 2] = I;
			//diff_Y= (Y - I)*(Y - I);
			//outputImg[j * stride + 3 * i + 0] = (unsigned char)(diff_Y > 255 ? 255 : (diff_Y < 0 ? 0 : diff_Y));
			//outputImg[j * stride + 3 * i + 1] = (unsigned char)(diff_Y > 255 ? 255 : (diff_Y < 0 ? 0 : diff_Y));
			//outputImg[j * stride + 3 * i + 2] = (unsigned char)(diff_Y > 255 ? 255 : (diff_Y < 0 ? 0 : diff_Y));
			///*Y = Y + 20;
			//R = (Y + 100) + 1.402 * Cr;
			//G = (Y + 100) - 0.714 * Cr - 0.344 *Cb;
			//B = (Y + 100) + 1.772 * Cb;
			//outputImg[j * stride + 3 * i + 2] = (unsigned char)(R > 255 ? 255 : (R < 0 ? 0 : R));
			//outputImg[j * stride + 3 * i + 1] = (unsigned char)(G > 255 ? 255 : (G < 0 ? 0 : G));
			//outputImg[j * stride + 3 * i + 0] = (unsigned char)(B > 255 ? 255 : (B < 0 ? 0 : B));*/
			////printf("%lf", Y);
		}
	}

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
