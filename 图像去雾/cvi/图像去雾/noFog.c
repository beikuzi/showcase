#include <cvirte.h>		
#include <userint.h>
#include <analysis.h> 
#include <stdio.h>
#include <nivision.h>
#include <time.h>
#include "noFog.h"

static int panelHandle;
static Image *SourceImage,*RImage,*BImage,*GImage,*TRImage,*TBImage,*TGImage;
static Image *DestImage2;//均衡图
static Image *DarkImage,*LowImage,*TempImage,*DestImage;//暗通道
static int width,height;

inline void img_put(Image *img,int window_num,char *window_name){//输出图像
	imaqSetWindowTitle (window_num, window_name);
 	imaqMoveWindow (window_num, imaqMakePoint(50,260));   
 	imaqDisplayImage (img, window_num, TRUE);
}
inline int get_pt(Image *img,int x,int y){//得知图像在x,y处的灰度值是多少
	Point pt;
	PixelValue P_val;
	pt.x=x,pt.y=y;
	imaqGetPixel(img,pt,&P_val);
	return P_val.grayscale; 
}
inline void set_pt(Image *img,int x,int y,int val){//设定图像x，y处的灰度值
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
void out(long long x,char* s){//在TEXTBOX上输出时间
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
				img_put(SourceImage,0,"原图像");
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
			
			for(int i=0;i<256;i++){//初始化
				sum[i][0]=sum[i][1]=sum[i][2]=0;
			}
			for(int j=0;j<height;j++){//将整张图片，三个通道不同灰度数量统计起来
				for(int i=0;i<width;i++){
					sum[get_pt(RImage,i,j)][0]++;//R
					sum[get_pt(GImage,i,j)][1]++;//G
					sum[get_pt(BImage,i,j)][2]++;//B
				}
			}
			for(int i=1;i<256;i++){//根据积分，后面的加上前面的
				for(int j=0;j<3;j++){
					sum[i][j]+=sum[i-1][j];
				}
			}
			int N=width*height,L=256;
			for(int i=0;i<256;i++){//变换公式
				for(int j=0;j<3;j++){
					T[i][j]=1.0*(double)(L-1)/(double)(N)*(double)(sum[i][j])+0.5;
				}
			}
			
			for(int j=0;j<height;j++){//在新的三个图上写入新灰度
				for(int i=0;i<width;i++){
					set_pt(TRImage,i,j,T[get_pt(RImage,i,j)][0]);
					set_pt(TGImage,i,j,T[get_pt(GImage,i,j)][1]); 
					set_pt(TBImage,i,j,T[get_pt(BImage,i,j)][2]); 
				}
			}
			imaqReplaceColorPlanes (DestImage2, SourceImage, IMAQ_RGB, TRImage,TGImage, TBImage); //三通道图片合成    
			img_put(DestImage2,7,"均值化最终图像"); 
			
			long long t2=time(NULL);
			out(t2-t1,"直方图均值化用时：");
			break;
	}
	return 0;
}


int low[1000][1000];//最低值
int temp[1000][1000];//临时记录数组 
int CVICALLBACK darkChannel  (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			long long t1=time(NULL); 
			double A=0;
			int r=7,d=2*r+1;//处理半径和直径
			double t0=0.1;
			double w=0.95;
			
			for(int j=0;j<height+r;j++){
				for(int i=0;i<width+r;i++){
					if(j>=height || i>=width){
						low[j][i]=255;//超出部分设为最大值
					}
					else{
						int u=get_pt(RImage,i,j),v=get_pt(GImage,i,j),w=get_pt(BImage,i,j);
						A=max( max(A,u),max(v,w) );//取全图像最亮的点作为大气层值A 
						int res=min( min(u,v) ,w );//取三个通道中最暗的那个
						low[j][i]=res;
					}
						
				}
			}
			
			
			int q[5000];//手写队列来实现单调队列
			for(int j=0;j<height+r;j++){//先对每一列往下进行单调队列
				int head=0,now=-1;//清空队列
				for(int i=0;i<width+r;i++){
					while(head<=now && q[head]+d<=i)head++;//如果超出了直径为d矩阵的范围，那么队列头弹出
					while(head<=now && low[j][q[now]]>=low[j][i])now--;//单调性，队列尾的位置比当前位置小，若他的值，比当前位置值还大，那么之后更不可能使用，扔掉
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
					
			imaqSetImageSize (DarkImage, width, height);//暗通道 
			imaqSetImageSize (TRImage, width, height);
			imaqSetImageSize (TGImage, width, height);
			imaqSetImageSize (TBImage, width, height);
			
			//此时的low是以(i,j)为右下角，直径为d的矩阵中的最小值，而暗通道以中间为中心，所以dark的中心(i,j)等于low的(i+r,j+r)
			for(int j=0;j<height;j++){
				for(int i=0;i<width;i++){
					//set_pt(DarkImage,i,j,get_pt(LowImage,i+r,j+r));
					set_pt(DarkImage,i,j,low[j+r][i+r]);
				}
			}

			
			for(int j=0;j<height;j++){
				for(int i=0;i<width;i++){
					double res=get_pt(DarkImage,i,j); //暗通道I(x)
					double temp=1.0-w*res/A;
					int r=1.0*(get_pt(RImage,i,j)-A)/((temp>t0)?temp:t0)+A+0.5;
					int g=1.0*(get_pt(GImage,i,j)-A)/((temp>t0)?temp:t0)+A+0.5;
					int b=1.0*(get_pt(BImage,i,j)-A)/((temp>t0)?temp:t0)+A+0.5; 
					set_pt(TRImage,i,j,r);
					set_pt(TGImage,i,j,g); 
					set_pt(TBImage,i,j,b); 
				}
			}
			//img_put(DarkImage,8,"暗通道");
			imaqReplaceColorPlanes (DestImage, SourceImage, IMAQ_RGB, TRImage,TGImage, TBImage); //三通道图片合成
			img_put(DestImage,9,"暗通道雾图处理最终图像");
			
			long long t2=time(NULL);
			out(t2-t1,"暗通道用时：");
			
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
