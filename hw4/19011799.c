#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>

int main()
{
	BITMAPFILEHEADER bmpFile;
	BITMAPINFOHEADER bmpInfo;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("test.bmp", "rb");
	fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	int width = bmpInfo.biWidth;
	int height = bmpInfo.biHeight;
	int size = bmpInfo.biSizeImage;
	int bitCnt = bmpInfo.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
	unsigned char* Y1, *Y2;

	unsigned char* inputImg = NULL, *outputImg_up = NULL;
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
	Y2 = (unsigned char*)malloc(sizeof(unsigned char) * (width2 * height2));
	outputImg_up = (unsigned char*)calloc(size2, sizeof(unsigned char));

	//-------------------------------------------업샘플링 코드--------------------------------------------------------
	double inter_filter[5][2] = { {1,0},{10.0 / 13,3.0 / 13},{2.0 / 4, 2.0 / 4},{3.0 / 13,10.0 / 13}, {0, 1} };//수직, 수평 관련 연산 할때 사용하는 필터
	double dia_filter[3] = { 7.0 / 16, 7.0 / 16, 2.0 / 16 };//대각선 관련 연산 할때 사용하는 필터
	double u, d, l, r, d1, d2;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (j < height - 1 && i < width - 1)
			{
				//2*2 개의 픽셀을 기반으로 21개의 픽셀을 뽑아내는 코드
				//편의상 알려진 픽셀의 값을 abcd라고 하겠습니다.
				/*
				a000b
				01210
				02320
				01210
				c000d
				0으로 되어 있는 픽셀은 bilinear interpolation을 사용하였습니다. 주변 픽셀에 가중치를 주어서 계산하는 방식을 사용했습니다.
				가중치로는 다음과 같이 주었을때 가장 좋은 값이 나와서 이렇게 필터를 설정하였습니다.{ {1,0},{10.0 / 13,3.0 / 13},{2.0 / 4, 2.0 / 4},{3.0 / 13,10.0 / 13}, {0, 1} }
				
				1번으로 되어 있는 픽셀들은 그 픽셀이 포함되어 있는 대각선의 원래 알고 있는 픽셀값들과 반대 대각선 픽셀 값들의 평균으로 가중치를 주어서 구하였습니다.
				가중치는 { 7.0 / 16, 7.0 / 16, 2.0 / 16 }로 주었습니다.

				2번으로 되어 있는 픽셀들은 각 변의 가운데 값들의 가중 평균으로 구하였습니다. 
				0번과 똑같이 inter_filter를 사용

				3번 픽셀은 a,b,c,d의 평균 값으로 채워 넣었습니다.
				*/
				u = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];//a,b의 평균
				d = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];//c,d의 평균
				l = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];//a,c의 평균
				r = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];//b,d의 평균
				d1 = (Y1[(j)* width + (i)] + Y1[(j + 1) * width + (i + 1)] * 1.0) / 2.0;//a,d의 평균
				d2 = (Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] * 1.0) / 2.0;//b,c의 평균

				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];//원래 픽셀 값
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];//a와 b의 가중평균 값
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];//a와 b의 가중평균 값
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];//a와 b의 가중평균 값
				Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];//원래 픽셀 값

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];//a와 c의 가중평균 값
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * dia_filter[0] + d2 * dia_filter[1] + Y1[(j + 1) * width + (i + 1)] * dia_filter[2];//a,d,(b+c)/2의 가중 평균 값
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];//u,d의 가중평균
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * dia_filter[0] + d1 * dia_filter[1] + Y1[(j + 1) * width + (i)] * dia_filter[2];//b,c,(a+d)/2의 가중 평균 값
				Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];//b와 d의 가중평균 값

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];//a와 c의 가중평균 값
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];//l,r의 가중평균
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;//a,b,c,d의 평균 값
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];//r,l의 가중평균
				Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];//b와 d의 가중평균 값

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];//a와 c의 가중평균 값
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * dia_filter[0] + d1 * dia_filter[1] + Y1[(j)* width + (i + 1)] * dia_filter[2];//c,b,(a+d)/2의 가중 평균 값
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];//d,u의 가중평균
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i + 1)] * dia_filter[0] + d2 * dia_filter[1] + Y1[(j)* width + (i)] * dia_filter[2];//d,a,(b+c)/2의 가중 평균 값
				Y2[(j * 4 + 3) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];//b와 d의 가중평균 값

				Y2[(j * 4 + 4) * width2 + (i * 4)] = Y1[(j + 1) * width + (i)];//원래 픽셀 값
				Y2[(j * 4 + 4) * width2 + (i * 4 + 1)] = Y1[(j + 1) * width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];//c와 d의 가중평균 값
				Y2[(j * 4 + 4) * width2 + (i * 4 + 2)] = Y1[(j + 1) * width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];//c와 d의 가중평균 값
				Y2[(j * 4 + 4) * width2 + (i * 4 + 3)] = Y1[(j + 1) * width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[3][1];//c와 d의 가중평균 값
				Y2[(j * 4 + 4) * width2 + (i * 4 + 4)] = Y1[(j + 1) * width + (i)];//원래 픽셀 값
			}

			//4개의 픽셀을 가지고 늘리면 마지막 가장 자리 부분에서 픽셀이 부족해져서 4개로 늘릴 수 없습니다. 따라서 가장 자리 부분은 위의 작업을 통해 
			//채워 넣은 픽셀을 복사하는 식으로 진행하였습니다.
			else if (j == height - 1 && i < width - 1)
			{
				//세로 방향 가장자리를 채우는 코드
				//마지막으로 채워진 행들의 값들을 세로로 복사해서 가장자리를 채웠습니다.
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
				//가로 방향 가장자리
				//마지막으로 채워진 열들의 값들을 가로로 복사해서 가장자리를 채웠습니다. 
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
			//모서리 가장자리를 채우는 코드
			//알려진 마지막 한개의 픽셀로 모서리 쪽 가장자리를 채워주었습니다.
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
		}
	}
	//----------------------------------------업샘플링--------------------------------------------------

	for (int j = 0; j < height2; j++)
	{
		for (int i = 0; i < width2; i++)
		{
			outputImg_up[j * stride2 + 3 * i + 0] = (unsigned char)Y2[j * width2 + i];
			outputImg_up[j * stride2 + 3 * i + 1] = (unsigned char)Y2[j * width2 + i];
			outputImg_up[j * stride2 + 3 * i + 2] = (unsigned char)Y2[j * width2 + i];
		}
	}


	FILE* outputFile_up = fopen("19011799.bmp", "wb");

	bmpInfo.biWidth = width2;
	bmpInfo.biHeight = height2;
	bmpInfo.biSizeImage = size2;
	bmpFile.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size2;
	fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile_up);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile_up);
	fwrite(outputImg_up, sizeof(unsigned char), size2, outputFile_up);

	free(inputImg);
	free(outputImg_up);

	free(Y1);
	free(Y2);

	fclose(inputFile1);
	fclose(outputFile_up);
	return 0;
}
