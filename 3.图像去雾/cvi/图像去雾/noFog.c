#include <cvirte.h>		
#include <userint.h>
#include <analysis.h> 
#include <stdio.h>
#include <nivision.h>
#include <time.h>
#include "noFog.h"

static int panelHandle;
static Image *SourceImage,*RImage,*BImage,*GImage,*TRImage,*TBImage,*TGImage;
static Image *DestImage2;//����ͼ
static Image *DarkImage,*LowImage,*TempImage,*DestImage;//��ͨ��
static int width,height;

inline void img_put(Image *img,int window_num,char *window_name){//���ͼ��
	imaqSetWindowTitle (window_num, window_name);
 	imaqMoveWindow (window_num, imaqMakePoint(50,260));   
 	imaqDisplayImage (img, window_num, TRUE);
}
inline int get_pt(Image *img,int x,int y){//��֪ͼ����x,y���ĻҶ�ֵ�Ƕ���
	Point pt;
	PixelValue P_val;
	pt.x=x,pt.y=y;
	imaqGetPixel(img,pt,&P_val);
	return P_val.grayscale; 
}
inline void set_pt(Image *img,int x,int y,int val){//�趨ͼ��x��y���ĻҶ�ֵ
	Point pt;
	PixelValue P_val;
	P_val.grayscale=val;
	pt.x=x,pt.y=y;
	imaqSetPixel(img,pt,P_val);
}

int min(int x,int y){
	return (x<y)?x:y;
}
int max(int x,int y){
	return (x>y)?x:y;
}
void out(long long x,char* s){//��TEXTBOX�����ʱ��
	SetCtrlVal(panelHandle, PANEL_TEXTBOX,s);
	int arr[10],t=0;
	char temp[10]="",t2=0;
	if(x==0)temp[0]='0';
	while(x){
		t++;
		arr[t]=(int)x%10;
		x/=10;
	}
	
	for(;t>=1;t--){
		temp[t2++]=(char)(arr[t]+48);	
	}
	SetCtrlVal(panelHandle, PANEL_TEXTBOX,temp);
	SetCtrlVal(panelHandle, PANEL_TEXTBOX," sec\n"); 
}

int CVICALLBACK read (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			char fileName[512];
			int status= FileSelectPopup ("", "*.*", "", "", VAL_LOAD_BUTTON, 0, 0, 1, 0, fileName);
			if(status==1){
				imaqReadFile (SourceImage, fileName, NULL, NULL);
				img_put(SourceImage,0,"ԭͼ��");
				imaqGetImageSize (SourceImage, &width, &height);
				imaqSetImageSize (DestImage, width, height);
				imaqExtractColorPlanes (SourceImage, IMAQ_RGB, RImage,GImage, BImage);
			}
			break;
	}
	return 0;
}

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "noFog.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	SourceImage= imaqCreateImage (IMAQ_IMAGE_RGB,2);
	RImage=  imaqCreateImage (IMAQ_IMAGE_U8,2);
	GImage=  imaqCreateImage (IMAQ_IMAGE_U8,2);
	BImage=  imaqCreateImage (IMAQ_IMAGE_U8,2);
	TRImage=  imaqCreateImage (IMAQ_IMAGE_U8,2);
	TGImage=  imaqCreateImage (IMAQ_IMAGE_U8,2);
	TBImage=  imaqCreateImage (IMAQ_IMAGE_U8,2);
	DarkImage=  imaqCreateImage (IMAQ_IMAGE_U8,2); 
	LowImage=  imaqCreateImage (IMAQ_IMAGE_U8,2);
	TempImage=  imaqCreateImage (IMAQ_IMAGE_U8,2);
	DestImage= imaqCreateImage (IMAQ_IMAGE_RGB,2); 
	DestImage2= imaqCreateImage (IMAQ_IMAGE_RGB,2);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int sum[256][3];
int T[256][3];
int CVICALLBACK equalization (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			long long t1=time(NULL);
			
			imaqSetImageSize (TRImage, width, height);
			imaqSetImageSize (TGImage, width, height);
			imaqSetImageSize (TBImage, width, height);
			
			for(int i=0;i<256;i++){//��ʼ��
				sum[i][0]=sum[i][1]=sum[i][2]=0;
			}
			for(int j=0;j<height;j++){//������ͼƬ������ͨ����ͬ�Ҷ�����ͳ������
				for(int i=0;i<width;i++){
					sum[get_pt(RImage,i,j)][0]++;//R
					sum[get_pt(GImage,i,j)][1]++;//G
					sum[get_pt(BImage,i,j)][2]++;//B
				}
			}
			for(int i=1;i<256;i++){//���ݻ��֣�����ļ���ǰ���
				for(int j=0;j<3;j++){
					sum[i][j]+=sum[i-1][j];
				}
			}
			int N=width*height,L=256;
			for(int i=0;i<256;i++){//�任��ʽ
				for(int j=0;j<3;j++){
					T[i][j]=1.0*(double)(L-1)/(double)(N)*(double)(sum[i][j])+0.5;
				}
			}
			
			for(int j=0;j<height;j++){//���µ�����ͼ��д���»Ҷ�
				for(int i=0;i<width;i++){
					set_pt(TRImage,i,j,T[get_pt(RImage,i,j)][0]);
					set_pt(TGImage,i,j,T[get_pt(GImage,i,j)][1]); 
					set_pt(TBImage,i,j,T[get_pt(BImage,i,j)][2]); 
				}
			}
			imaqReplaceColorPlanes (DestImage2, SourceImage, IMAQ_RGB, TRImage,TGImage, TBImage); //��ͨ��ͼƬ�ϳ�    
			img_put(DestImage2,7,"��ֵ������ͼ��"); 
			
			long long t2=time(NULL);
			out(t2-t1,"ֱ��ͼ��ֵ����ʱ��");
			break;
	}
	return 0;
}


int low[1000][1000];//���ֵ
int temp[1000][1000];//��ʱ��¼���� 
int CVICALLBACK darkChannel  (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			long long t1=time(NULL); 
			double A=0;
			int r=7,d=2*r+1;//����뾶��ֱ��
			double t0=0.1;
			double w=0.95;
			
			for(int j=0;j<height+r;j++){
				for(int i=0;i<width+r;i++){
					if(j>=height || i>=width){
						low[j][i]=255;//����������Ϊ���ֵ
					}
					else{
						int u=get_pt(RImage,i,j),v=get_pt(GImage,i,j),w=get_pt(BImage,i,j);
						A=max( max(A,u),max(v,w) );//ȡȫͼ�������ĵ���Ϊ������ֵA 
						int res=min( min(u,v) ,w );//ȡ����ͨ��������Ǹ�
						low[j][i]=res;
					}
						
				}
			}
			
			
			int q[5000];//��д������ʵ�ֵ�������
			for(int j=0;j<height+r;j++){//�ȶ�ÿһ�����½��е�������
				int head=0,now=-1;//��ն���
				for(int i=0;i<width+r;i++){
					while(head<=now && q[head]+d<=i)head++;//���������ֱ��Ϊd����ķ�Χ����ô����ͷ����
					while(head<=now && low[j][q[now]]>=low[j][i])now--;//�����ԣ�����β��λ�ñȵ�ǰλ��С��������ֵ���ȵ�ǰλ��ֵ������ô֮���������ʹ�ã��ӵ�
					q[++now]=i;
					temp[j][i]=low[j][q[head]];
				}
			}
			for(int i=0;i<width+r;i++){
				int head=0,now=-1;
				for(int j=0;j<height+r;j++){
					while(head<=now && q[head]+d<=j)head++;
					while(head<=now && temp[q[head]][i]>=temp[j][i] )now--;
					q[++now]=j;
					low[j][i]=temp[q[head]][i];
				}
			}
					
			imaqSetImageSize (DarkImage, width, height);//��ͨ�� 
			imaqSetImageSize (TRImage, width, height);
			imaqSetImageSize (TGImage, width, height);
			imaqSetImageSize (TBImage, width, height);
			
			//��ʱ��low����(i,j)Ϊ���½ǣ�ֱ��Ϊd�ľ����е���Сֵ������ͨ�����м�Ϊ���ģ�����dark������(i,j)����low��(i+r,j+r)
			for(int j=0;j<height;j++){
				for(int i=0;i<width;i++){
					//set_pt(DarkImage,i,j,get_pt(LowImage,i+r,j+r));
					set_pt(DarkImage,i,j,low[j+r][i+r]);
				}
			}

			
			for(int j=0;j<height;j++){
				for(int i=0;i<width;i++){
					double res=get_pt(DarkImage,i,j); //��ͨ��I(x)
					double temp=1.0-w*res/A;
					int r=1.0*(get_pt(RImage,i,j)-A)/((temp>t0)?temp:t0)+A+0.5;
					int g=1.0*(get_pt(GImage,i,j)-A)/((temp>t0)?temp:t0)+A+0.5;
					int b=1.0*(get_pt(BImage,i,j)-A)/((temp>t0)?temp:t0)+A+0.5; 
					set_pt(TRImage,i,j,r);
					set_pt(TGImage,i,j,g); 
					set_pt(TBImage,i,j,b); 
				}
			}
			//img_put(DarkImage,8,"��ͨ��");
			imaqReplaceColorPlanes (DestImage, SourceImage, IMAQ_RGB, TRImage,TGImage, TBImage); //��ͨ��ͼƬ�ϳ�
			img_put(DestImage,9,"��ͨ����ͼ��������ͼ��");
			
			long long t2=time(NULL);
			out(t2-t1,"��ͨ����ʱ��");
			
			break;
	}
	return 0;
}
int CVICALLBACK quit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface (0);
			break;
	}
	return 0;
}
