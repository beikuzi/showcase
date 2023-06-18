#include <utility.h>
#include <ansi_c.h>
#include "nivision.h"
#include <cvirte.h>	
#include <string.h>
#include <stdio.h>
#include <userint.h>
#include "recognition.h"

static int panelHandle;
#define and &&
#define or ||

static Image *SourceImage,*DestImage,*OutImage,*CheckImage,*OutImage2;
inline void img_put(Image *img,int window_num,char *window_name){//输出图像
	imaqSetWindowTitle (window_num, window_name);
 	imaqMoveWindow (window_num, imaqMakePoint(0,0));//  窗口号0~15,窗口左上角坐标   
 	imaqDisplayImage (img, window_num, TRUE);//窗口号，窗口是否可变
}
inline int get_pt(Image *img,int x,int y){//得知图像在x,y处的值是多少
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

int flag=0;//是否输出图像，0输出，1不输出

#define maxn 2000
//假设图像行和列的像素上限是2000
int queue[maxn],tot=0;//用来记录行分割的位置


int arr[maxn][maxn];//记录对应位置的黑白像素
int sum[maxn][maxn];//二维前缀和
int ask(int x1,int y1,int x2,int y2){ //询问二维区间像素数量
	return sum[x2][y2]-sum[x2][y1-1]-sum[x1-1][y2]+sum[x1-1][y1-1];
}

#define maxm 10000
#define maxt 36
//假设匹配的汉字数上限是10000,汉字图像大小是36
long long val[maxm][maxt];//匹配值
char hanzi[maxm][10];//汉字词

#define maxs 512 
char road_txt[maxs];  //匹配txt的路径
char filename[maxs];

int cnt=-1;//从txt中读入的汉字数量
void deal(){
	 //读入匹配的汉字和对应的匹配值
	GetProjectDir(road_txt); 
	strcat(road_txt,"\\txt\\");
	
	strcpy(filename,road_txt);
	strcat(filename,"table.txt");
	FILE* df1 = fopen(filename,"r");//document flow文件流
	while(fscanf(df1,"%s",hanzi[++cnt])!=EOF){
		//printf("%s\n",hanzi[cnt]);  
	}
	//printf("%d\n",k);
	
	
	strcpy(filename,road_txt);
	strcat(filename,"match.txt");
	FILE* df2 = fopen(filename,"r"); 
	for(int i=0;i<cnt;i++){
		for(int j=0;j<maxt;j++){
			fscanf(df2,"%lld",&val[i][j]);
		}
	}
}
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "recognition.uir", PANEL)) < 0)
		return -1;
	deal();
	DisplayPanel (panelHandle);
	SourceImage= imaqCreateImage (0,2);
	DestImage= imaqCreateImage (0,2);  
	OutImage= imaqCreateImage (0,2);//行分割 
	CheckImage= imaqCreateImage (0,2);//单个字 
	OutImage2= imaqCreateImage (0,2);//列分割
	
	RunUserInterface ();
	DiscardPanel (panelHandle);
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

int status=0;
int width=0,height=0;
int CVICALLBACK load_file (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			int thre_val=250;
			const int maxlen=512;
			char fileName[maxlen];
			status= FileSelectPopup ("", "*.*", "", "选一个图像", VAL_LOAD_BUTTON, 0, 0, 1, 0, fileName); //为1时打开正常
			if(status==1){
				imaqReadFile (SourceImage, fileName, NULL, NULL);
				if(!flag)img_put(SourceImage,0,"原图像");
				imaqGetImageSize (SourceImage, &width, &height);
				imaqSetImageSize (DestImage, width, height); 
				imaqSetImageSize (OutImage, width, height);
				imaqSetImageSize (OutImage2, width, height);
				
				imaqThreshold(DestImage, SourceImage, thre_val,255,TRUE, 255) ;
				if(!flag)img_put(DestImage,1,"二值化图像");  
				
				//前缀和
				for(int i=1;i<=width;i++){
					for(int j=1;j<=height;j++){
						int val=get_pt(DestImage,i-1,j-1);
						if(val>250) arr[i][j]=0;//白色记为0
						else arr[i][j]=1; //黑色记为1
					
						sum[i][j]=sum[i-1][j]+sum[i][j-1]-sum[i-1][j-1]+arr[i][j];
					
						set_pt(OutImage,i-1,j-1,arr[i][j]*255);//反色输出
						set_pt(OutImage2,i-1,j-1,arr[i][j]*255);
					}
				}
			}
			break;
	}
	return 0;
}

int CVICALLBACK row_cut (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:

			tot=0;
			//行分割
			int match=1;//标记是行开始还是行结束
			for(int j=1;j<=height;j++){
				int v=ask(1,j,width,j);//询问这行的黑色像素个数
				if(match==1 and v!=0){//行开始有元素
					match*=-1;
					queue[++tot]=j;//用queue两个一组记录
				}
				else if(match==-1 and v==0){//行结束没有元素
					match*=-1;
					queue[++tot]=j;
				}
			}
			
			//将行分割展示在反色图上
			for(int v=1;v<=tot;v++){
				int j=queue[v];
				for(int i=1;i<=width;i++){
					set_pt(OutImage,i-1,j-1,255);
					set_pt(OutImage2,i-1,j-1,255); 
				}
				//printf("%d\n",queue[v]);
			}
			if(!flag)img_put(OutImage,2,"行分割图像"); 
			break;
	}
	return 0;
}


int getcnt(long long x){//对于一个值，求出他二进制里有多少个一
	int ct=0;
	while(x){
		ct++;
		x-=(x&(-x));//lowbit
	}
	return ct;
}

int minloc,mindis;//最小模板位置，以及最小差距，赋极大值
void check(int l,int r,int u,int d){//对于图上区域lrud，检查他对模板库的匹配
	int len=36;
	Rect rect;
	rect.top=u,
	rect.left=l,
	rect.width=r-l+1,
	rect.height=d-u+1;
	imaqSetImageSize (CheckImage, len,len);  
	imaqResample(CheckImage,DestImage,len,len,1,rect);
//	img_put(CheckImage,3,"分割图像");
	
	long long cut[36];//被裁出来的汉字储存在这  
	for(int i=0;i<len;i++){				//height
		cut[i]=0;
		for(int j=0;j<len;j++){			//width
			int val=get_pt(CheckImage,j,i); 	  //先width后height
			if(val<114)cut[i]|=(1ll<<j);//是黑色，算入
		}
	}
	
	minloc=-1,mindis=114514;	
	for(int i=0;i<cnt;i++) {
		int res=0;
		for(int j=0;j<len;j++){
			long long x=cut[j]^val[i][j];
			res+=getcnt(x);
			if(res>mindis)break;//剪枝优化
		}
		if(res<mindis){
			minloc=i;
			mindis=res;
		}
	}
}

int q1[maxn],t1=0;//初次列分割位置 
int q2[maxn],t2=0;//重新列分割文职
int CVICALLBACK col_cut (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{

	switch (event)
	{
		case EVENT_COMMIT:
			for(int t=1;t<=tot;t+=2){
				t1=0,t2=0;
				
				//列处理
				int i=queue[t],j=queue[t+1];
				int match=1;
				for(int k=1;k<=width;k++){
					int v=ask(k,i,k,j);//询问第k列从第i行到第j行的黑色元素
					if(match==1 and v!=0){//列开始出现元素
						match*=-1;
						q1[++t1]=k;
					}
					else if(match==-1 and v==0){//列结束没有元素
						match*=-1;
						q1[++t1]=k;
					}
				}
				//贪心出正确的列分割
				int nowloc,nowdis;
				for(int k=2;k<=t1;k+=2){
					int x=i,y=j;//x是行首，y是行尾,q[k]是列首，q[k+1]是列尾   
					
					//得出当前分割的情况
					int nowl=q1[k-1],nowr=q1[k];
					while(ask(nowl,x,nowr,x)==0)x++;
					while(ask(nowl,y,nowr,y)==0)y--;
					check(nowl-1,nowr-1,x-1,y-1);
					nowloc=minloc;
					nowdis=mindis;
					//if(k==4)while(1);
					//不断往右贪心尝试是否可行
					while(k+2<=t1){
						int l=q1[k+1],r=q1[k+2];
						if((r+l)/2-(nowr+nowl)/2>j-i)break;//重心的距离应该不比行高度还远
						x=i,y=j;
						while(ask(nowl,x,r,x)==0)x++;
						while(ask(nowl,y,r,y)==0)y--;
						check(nowl-1,r-1,x-1,y-1);
						if(mindis<nowdis){//贪心可行
							k+=2;
							nowr=r;
							nowdis=mindis;
							nowloc=minloc;
							//while(1); 
						}
						else break;
					}
					//用新的队列记录
					q2[++t2]=nowl;
					q2[++t2]=nowr;
					//输出答案
					char s[10];
					strcpy(s,hanzi[nowloc]);
					SetCtrlVal (panelHandle, PANEL_out_text,s );
					
				}
				
				//反色输出
				for(int k=1;k<=t2;k+=2){//两个一组记录列位置
					for(int v=i;v<j;v++){
						set_pt(OutImage2,q2[k]-1,v-1,255);
						set_pt(OutImage2,q2[k+1]-1,v-1,255);
					}
				}
				SetCtrlVal (panelHandle, PANEL_out_text,"\n" ); 
			}
			SetCtrlVal (panelHandle, PANEL_out_text,"\n" );
			if(!flag)img_put(OutImage2,3,"列分割图像");
			break;
	}
	return 0;
}


int CVICALLBACK finish (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	 
	switch (event)
	{
		case EVENT_COMMIT:
			flag=1;
			load_file(panel,control,event,callbackData,eventData1,eventData2);    
			if(status==1){
				row_cut(panel,control,event,callbackData,eventData1,eventData2);
				col_cut(panel,control,event,callbackData,eventData1,eventData2); 
				
			}
			flag=0; 
			break;
	}
	return 0;
}
