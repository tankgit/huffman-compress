#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "cv.h"
#include "highgui.h"

int counts[256];
double freq[256];
char coding[256][256]; 
int width,height;
double total;
double sum;
int pn,qn;

typedef struct HNode{
	double weight;
	int p,l,r;
}HNode;
HNode node[512];


void count(IplImage* img);

void huffman();

void find(int n);

void output(IplImage* img,char* argv);

void compression(char* argv);

void table(char* argv);

int main(int argc,char** argv)
{
	int i,j;
	double l=0;
	IplImage* img;
	if(argc==2 && (img = cvLoadImage(argv[1],0))!=0)
	{
		printf("\033[01;31mCompression start.\033[0m\n\n");
		width=img->width;
		height=img->height;
		total=(double)width*height;
		count(img);
		for(i=0;i<256;i++)
		{	
			freq[i]=1*((double)counts[i])/total;
			sum+=freq[i];
		}
	
		huffman();
		for(i=0;i<256;i++)
		{
			//printf("%3d : %-7d  %-1.6lf    %-2d    %s\n",i,counts[i],freq[i],(int)strlen(coding[i]),coding[i]);
			l+=(double)strlen(coding[i])*freq[i];
		}	
		output(img,argv[1]);
		compression(argv[1]);
		table(argv[1]);
		printf("\n\033[01;31mProgram executed successfully!\033[0m\n\n");
		printf("\033[34mCompression Info : \n");
		printf("\nImage size : %.0lf pix (= %d x %d)\nOrigin : %.4lf MB\nCompress : %.4lf MB\nAverage Bit : %lf\nCompression ratio : %2.2lf %%\033[0m\n",total,width,height,total/1024/1024,total/1024/1024*l/8,l,(1-l/8)*100);

		cvReleaseImage(&img);
		remove(argv[1]);
		return 0;
	}
	else {
		printf("Usage:\n\t./LC [File Name]\n");
	}
	return -1;
}

void count(IplImage* img)
{
	printf("Start counting...\n");
	int i,j;
	for(i=0;i<256;i++)
	counts[i]=0;
	CvScalar s;
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			s=cvGet2D(img,i,j);
			counts[(int)s.val[0]]++;
		}
	printf("Finished!\n");
	return;
}

void huffman()
{
	printf("Huffman encoding...\n");
	int i,j;
	for(i=0;i<256;i++)
	{
		node[i].weight=freq[i];
		node[i].p=-1;
		node[i].l=-1;
		node[i].r=-1;
	}
	for(;i<512;i++)
    {
        node[i].weight=0;
        node[i].p=-1;
        node[i].l=-1;
        node[i].r=-1;
    }

	for(i=256;i<512;i++)
	{
		find(i);
		node[pn].p=i;
		node[qn].p=i;
		node[i].l=pn;
		node[i].r=qn;
		node[i].weight=node[pn].weight+node[qn].weight;
	}
	char cd[256];
	cd[255]='\0';
	for(i=0;i<256;i++)
	{
		int start=255;
		for(j=i;node[j].p!=-1;)
		{
			int k=node[j].p;
			if(node[k].l==j)cd[--start]='0';
			else cd[--start]='1';
			j=k;
		}
		strcpy(coding[i],&cd[start]);
	}
	printf("Finished!\n");
	return;
}

void find(int n)
{
	int i;
	double p=1.0,q=1.0;
	for(i=0;i<n;i++)
	{
		if(node[i].p==-1)
		{
			if(node[i].weight-p<0.0)
			{
				if(node[i].weight-q<0.0)
				{pn=qn;p=q;qn=i;q=node[i].weight;}
				else {pn=i;p=node[i].weight;}
			}
		}
	}
	return;
}

void output(IplImage* img,char* argv)
{
	printf("Mid-procedure...\n");
	int i,j;
	CvScalar s;
	char name[20];
	strcpy(name,argv);
	strcat(name,".tem");
	FILE* fp=fopen(name,"wb");
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			s=cvGet2D(img,i,j);
			fprintf(fp,"%s",coding[(int)s.val[0]]);
		}
	}
	fclose(fp);
	printf("Finished!\n");
	return;
}

void compression(char* argv)
{
	printf("Output compressed file...\n");
	int c;
	char buf[9];
	char cb;
	char name1[20];
	char name2[20];
	strcpy(name1,argv);
	strcat(name1,".tem");
	strcpy(name2,argv);
	strcat(name2,".lc");
	FILE* fp=fopen(name1,"r");
	FILE* fw=fopen(name2,"wb");
	while(fgets(buf,9,fp)!=NULL)
	{
		int i;
		c=0;
		for(i=7;i>=0;i--)
		c+=(buf[7-i]-48)*pow(2,i);
		cb=(char)c;
		fprintf(fw,"%c",cb);
	}
	fclose(fp);
	fclose(fw);
	remove(name1);
	printf("Finished!\n");
	return;
}

void table(char* argv)
{
	printf("Saving huffman table file...\n");
	int i;
	char name[20];
	strcpy(name,argv);
	strcat(name,".huff");
	FILE* fp=fopen(name,"w");
	fprintf(fp,"%d %d\n",width,height);
	for(i=0;i<512;i++)
	{
		fprintf(fp,"%d %d %d\n",node[i].p,node[i].l,node[i].r);
	}
	fclose(fp);
	printf("Finished!\n");
	return;
}
