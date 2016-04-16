#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "cv.h"
#include "highgui.h"

char bb[8];
char name[20];

typedef struct HNode{
	int p,l,r;
}HNode;
HNode node[512];

char* trans(char c);
void findname(char* argv);
void bin2txt(FILE* fp);
void decompress(FILE* huff);


int main(int argc,char** argv)
{
	printf("\033[01;31mDecompression start.\033[0m\n\n");
	int i;
	FILE* fp;
	FILE* huff;
	if(!(fp=fopen(argv[1],"rb")))
	{
		printf("No image source file!\n");
		return -1;
	}
	findname(argv[1]);
	char huffn[25];
	strcpy(huffn,name);
	strcat(huffn,".huff");
	if(!(huff=fopen(huffn,"r")))
	{
		printf("No huffman source file!\n");
		return -1;
	}

	bin2txt(fp);
	decompress(huff);
	printf("\n\033[01;31mImage decompressed successfully!\033[0m\n");
	fclose(fp);
	fclose(huff);
	remove(argv[1]);
	remove(huffn);
	return 0;
}

char* trans(char c)
{
	int i=8;
	while(i--)
	{
		bb[i]=(c&1)+48;
		c=c>>1;
	}
	return bb;
}

void findname(char* argv)
{
	int i;
	int l=strlen(argv);
	while(l--)
	if(argv[l]=='.')break;
	name[l]='\0';
	while(l--)
	name[l]=argv[l];
	return;
}

void bin2txt(FILE* fp)
{
	printf("Transform binary file to text file...\n");
	char bit[2];
	char mname[25];
	strcpy(mname,name);
	strcat(mname,".temd");
	FILE* out=fopen(mname,"w");
	while(fgets(bit,2,fp)!=NULL)
	fprintf(out,"%s",trans(bit[0]));
	fclose(out);
	printf("\033[33m[100%%]\033[0m Transform finished!\n");
	return;
}

void decompress(FILE* huff)
{
	printf("Execute the decompression...\n");
	int i;
	int width,height;
	fscanf(huff,"%d %d\n",&width,&height);
	for(i=0;i<512;i++)
	fscanf(huff,"%d %d %d\n",&node[i].p,&node[i].l,&node[i].r);
	char mname[25];
	strcpy(mname,name);
	strcat(mname,".temd");
	FILE* tem=fopen(mname,"r");
	CvSize size={width,height};
	printf("Creating the image file...\n");
	IplImage* img=cvCreateImage(size,IPL_DEPTH_8U,1);
	printf("\033[33m[100%%]\033[0m Finished!\n");
	CvScalar s;
	int k=0,j=0;
	char bit[2];
	i=511;
	printf("Saving the pixes to the image...\n");
	while(fgets(bit,2,tem)!=NULL)
	{
		
		if(node[i].l==-1)
		{
			s.val[0]=i;
			//if(j==height)break;
			//printf("%d %d\n",k,j);
			cvSet2D(img,j,k,s);
			k++;
			if(k==width)
			{
				k=0;
				j++;
			}
			i=511;
		}
		if(bit[0]-48)i=node[i].r;
		else i=node[i].l;
	}
	cvSaveImage(name,img,0);
	printf("\033[33m[100%%]\033[0m Saving finished!\n");
	cvReleaseImage(&img);
	fclose(tem);
	remove(mname);
	printf("\033[33m[100%%]\033[0m Decompression finished!\n");
	return;
}


