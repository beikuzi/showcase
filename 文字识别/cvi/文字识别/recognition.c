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
inline void img_put(Image *img,int window_num,char *window_name){//���ͼ��
	imaqSetWindowTitle (window_num, window_name);
 	imaqMoveWindow (window_num, imaqMakePoint(0,0));//  ���ں�0~15,�������Ͻ�����   
 	imaqDisplayImage (img, window_num, TRUE);//���ںţ������Ƿ�ɱ�
}
inline int get_pt(Image *img,int x,int y){//��֪ͼ����x,y����ֵ�Ƕ���
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

int flag=0;//�Ƿ����ͼ��0�����1�����

#define maxn 2000
//����ͼ���к��е�����������2000
int queue[maxn],tot=0;//������¼�зָ��λ��


int arr[maxn][maxn];//��¼��Ӧλ�õĺڰ�����
int sum[maxn][maxn];//��άǰ׺��
int ask(int x1,int y1,int x2,int y2){ //ѯ�ʶ�ά������������
	return sum[x2][y2]-sum[x2][y1-1]-sum[x1-1][y2]+sum[x1-1][y1-1];
}

#define maxm 10000
#define maxt 36
//����ƥ��ĺ�����������10000,����ͼ���С��36
long long val[maxm][maxt];//ƥ��ֵ
char hanzi[maxm][10];//���ִ�

#define maxs 512 
char road_txt[maxs];  //ƥ��txt��·��
char filename[maxs];

int cnt=-1;//��txt�ж���ĺ�������
void deal(){
	 //����ƥ��ĺ��ֺͶ�Ӧ��ƥ��ֵ
	GetProjectDir(road_txt); 
	strcat(road_txt,"\\txt\\");
	
	strcpy(filename,road_txt);
	strcat(filename,"table.txt");
	FILE* df1 = fopen(filename,"r");//document flow�ļ���
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
	OutImage= imaqCreateImage (0,2);//�зָ� 
	CheckImage= imaqCreateImage (0,2);//������ 
	OutImage2= imaqCreateImage (0,2);//�зָ�
	
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
			status= FileSelectPopup ("", "*.*", "", "ѡһ��ͼ��", VAL_LOAD_BUTTON, 0, 0, 1, 0, fileName); //Ϊ1ʱ������
			if(status==1){
				imaqReadFile (SourceImage, fileName, NULL, NULL);
				if(!flag)img_put(SourceImage,0,"ԭͼ��");
				imaqGetImageSize (SourceImage, &width, &height);
				imaqSetImageSize (DestImage, width, height); 
				imaqSetImageSize (OutImage, width, height);
				imaqSetImageSize (OutImage2, width, height);
				
				imaqThreshold(DestImage, SourceImage, thre_val,255,TRUE, 255) ;
				if(!flag)img_put(DestImage,1,"��ֵ��ͼ��");  
				
				//ǰ׺��
				for(int i=1;i<=width;i++){
					for(int j=1;j<=height;j++){
						int val=get_pt(DestImage,i-1,j-1);
						if(val>250) arr[i][j]=0;//��ɫ��Ϊ0
						else arr[i][j]=1; //��ɫ��Ϊ1
					
						sum[i][j]=sum[i-1][j]+sum[i][j-1]-sum[i-1][j-1]+arr[i][j];
					
						set_pt(OutImage,i-1,j-1,arr[i][j]*255);//��ɫ���
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
			//�зָ�
			int match=1;//������п�ʼ�����н���
			for(int j=1;j<=height;j++){
				int v=ask(1,j,width,j);//ѯ�����еĺ�ɫ���ظ���
				if(match==1 and v!=0){//�п�ʼ��Ԫ��
					match*=-1;
					queue[++tot]=j;//��queue����һ���¼
				}
				else if(match==-1 and v==0){//�н���û��Ԫ��
					match*=-1;
					queue[++tot]=j;
				}
			}
			
			//���зָ�չʾ�ڷ�ɫͼ��
			for(int v=1;v<=tot;v++){
				int j=queue[v];
				for(int i=1;i<=width;i++){
					set_pt(OutImage,i-1,j-1,255);
					set_pt(OutImage2,i-1,j-1,255); 
				}
				//printf("%d\n",queue[v]);
			}
			if(!flag)img_put(OutImage,2,"�зָ�ͼ��"); 
			break;
	}
	return 0;
}


int getcnt(long long x){//����һ��ֵ����������������ж��ٸ�һ
	int ct=0;
	while(x){
		ct++;
		x-=(x&(-x));//lowbit
	}
	return ct;
}

int minloc,mindis;//��Сģ��λ�ã��Լ���С��࣬������ֵ
void check(int l,int r,int u,int d){//����ͼ������lrud���������ģ����ƥ��
	int len=36;
	Rect rect;
	rect.top=u,
	rect.left=l,
	rect.width=r-l+1,
	rect.height=d-u+1;
	imaqSetImageSize (CheckImage, len,len);  
	imaqResample(CheckImage,DestImage,len,len,1,rect);
//	img_put(CheckImage,3,"�ָ�ͼ��");
	
	long long cut[36];//���ó����ĺ��ִ�������  
	for(int i=0;i<len;i++){				//height
		cut[i]=0;
		for(int j=0;j<len;j++){			//width
			int val=get_pt(CheckImage,j,i); 	  //��width��height
			if(val<114)cut[i]|=(1ll<<j);//�Ǻ�ɫ������
		}
	}
	
	minloc=-1,mindis=114514;	
	for(int i=0;i<cnt;i++) {
		int res=0;
		for(int j=0;j<len;j++){
			long long x=cut[j]^val[i][j];
			res+=getcnt(x);
			if(res>mindis)break;//��֦�Ż�
		}
		if(res<mindis){
			minloc=i;
			mindis=res;
		}
	}
}

int q1[maxn],t1=0;//�����зָ�λ�� 
int q2[maxn],t2=0;//�����зָ���ְ
int CVICALLBACK col_cut (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{

	switch (event)
	{
		case EVENT_COMMIT:
			for(int t=1;t<=tot;t+=2){
				t1=0,t2=0;
				
				//�д���
				int i=queue[t],j=queue[t+1];
				int match=1;
				for(int k=1;k<=width;k++){
					int v=ask(k,i,k,j);//ѯ�ʵ�k�дӵ�i�е���j�еĺ�ɫԪ��
					if(match==1 and v!=0){//�п�ʼ����Ԫ��
						match*=-1;
						q1[++t1]=k;
					}
					else if(match==-1 and v==0){//�н���û��Ԫ��
						match*=-1;
						q1[++t1]=k;
					}
				}
				//̰�ĳ���ȷ���зָ�
				int nowloc,nowdis;
				for(int k=2;k<=t1;k+=2){
					int x=i,y=j;//x�����ף�y����β,q[k]�����ף�q[k+1]����β   
					
					//�ó���ǰ�ָ�����
					int nowl=q1[k-1],nowr=q1[k];
					while(ask(nowl,x,nowr,x)==0)x++;
					while(ask(nowl,y,nowr,y)==0)y--;
					check(nowl-1,nowr-1,x-1,y-1);
					nowloc=minloc;
					nowdis=mindis;
					//if(k==4)while(1);
					//��������̰�ĳ����Ƿ����
					while(k+2<=t1){
						int l=q1[k+1],r=q1[k+2];
						if((r+l)/2-(nowr+nowl)/2>j-i)break;//���ĵľ���Ӧ�ò����и߶Ȼ�Զ
						x=i,y=j;
						while(ask(nowl,x,r,x)==0)x++;
						while(ask(nowl,y,r,y)==0)y--;
						check(nowl-1,r-1,x-1,y-1);
						if(mindis<nowdis){//̰�Ŀ���
							k+=2;
							nowr=r;
							nowdis=mindis;
							nowloc=minloc;
							//while(1); 
						}
						else break;
					}
					//���µĶ��м�¼
					q2[++t2]=nowl;
					q2[++t2]=nowr;
					//�����
					char s[10];
					strcpy(s,hanzi[nowloc]);
					SetCtrlVal (panelHandle, PANEL_out_text,s );
					
				}
				
				//��ɫ���
				for(int k=1;k<=t2;k+=2){//����һ���¼��λ��
					for(int v=i;v<j;v++){
						set_pt(OutImage2,q2[k]-1,v-1,255);
						set_pt(OutImage2,q2[k+1]-1,v-1,255);
					}
				}
				SetCtrlVal (panelHandle, PANEL_out_text,"\n" ); 
			}
			SetCtrlVal (panelHandle, PANEL_out_text,"\n" );
			if(!flag)img_put(OutImage2,3,"�зָ�ͼ��");
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
