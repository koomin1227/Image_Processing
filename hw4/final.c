void upsampling_basic_final()
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
	double dia_filter[3] = { 7.0 / 16,7.0 / 16,2.0 / 16 };
	double u, d, l, r,d1,d2;
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
				d1 = (Y1[(j)* width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 2.0;
				d2 = (Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)]) / 2.0;

				Y2[(j * 4) * width2 + (i * 4)] = Y1[(j)* width + (i)];
				Y2[(j * 4) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j)* width + (i + 1)] * inter_filter[1][1];
				Y2[(j * 4) * width2 + (i * 4 + 2)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j)* width + (i + 1)] * inter_filter[2][1];
				Y2[(j * 4) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j)* width + (i + 1)] * inter_filter[3][1];
				Y2[(j * 4) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)];

				Y2[(j * 4 + 1) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[1][0] + Y1[(j + 1) * width + (i)] * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 1)] = Y1[(j)* width + (i)] * dia_filter[0] + d2 * dia_filter[1] + Y1[(j + 1) * width + (i + 1)] * dia_filter[2];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 2)] = u * inter_filter[1][0] + d * inter_filter[1][1];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 3)] = Y1[(j)* width + (i + 1)] * dia_filter[0] + d1 * dia_filter[1] + Y1[(j + 1) * width + (i)] * dia_filter[2];
				Y2[(j * 4 + 1) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[1][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[1][1];

				Y2[(j * 4 + 2) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[2][0] + Y1[(j + 1) * width + (i)] * inter_filter[2][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 1)] = l * inter_filter[1][0] + r * inter_filter[1][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 2)] = (Y1[(j)* width + (i)] + Y1[(j)* width + (i + 1)] + Y1[(j + 1) * width + (i)] + Y1[(j + 1) * width + (i + 1)]) / 4;
				Y2[(j * 4 + 2) * width2 + (i * 4 + 3)] = l * inter_filter[3][0] + r * inter_filter[3][1];
				Y2[(j * 4 + 2) * width2 + (i * 4 + 4)] = Y1[(j)* width + (i + 1)] * inter_filter[2][0] + Y1[(j + 1) * width + (i + 1)] * inter_filter[2][1];

				Y2[(j * 4 + 3) * width2 + (i * 4)] = Y1[(j)* width + (i)] * inter_filter[3][0] + Y1[(j + 1) * width + (i)] * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 1)] = Y1[(j+1)* width + (i)] * dia_filter[0] + d1 * dia_filter[1] + Y1[(j) * width + (i + 1)] * dia_filter[2];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 2)] = u * inter_filter[3][0] + d * inter_filter[3][1];
				Y2[(j * 4 + 3) * width2 + (i * 4 + 3)] = Y1[(j + 1)* width + (i + 1)] * dia_filter[0] + d2 * dia_filter[1] + Y1[(j) * width + (i)] * dia_filter[2];
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

			}
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
