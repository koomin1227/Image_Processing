#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *a,*p;
	FILE *fp1,*fp2;
	int len,tot;
    int d[10]={48,49,50,51,52,53,54,55,56,57};

	fp1 = fopen("output.txt","rb");
	if (fp1 == NULL)
		printf("error");
	// fp2 = fopen("output.txt","wb");
	//파일 앞의 몇개 들어 있는지 정보 추출
	//fscanf(fp1,"%d %d ",&len,&tot);
	//정보로 동적할당
	a = (int *) malloc(sizeof(int) * tot);
	p = a;
	//정보만큼 파일 읽기
	//printf("%d %d\n",len,tot);
	fread(a,sizeof(int),10,fp1);

	//파일 출력
	p=a;
	// for(int i=0;i<tot/len;i++)
	// {
	// 	fwrite(p,sizeof(char),5,fp2);
    //     fwrite(" ",sizeof(char),1,fp2);
	// 	p = p + len;
	// }
	for(int i=0;i<10;i++)
	{
		printf("%d ",a[i]);
	}
    //fwrite(d,sizeof(int),10,fp2);
	fclose(fp1);
	printf("\nendqw1");
}