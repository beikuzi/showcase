#include <sstream>
#include <iostream>
#include <fstream>
#include <ros/ros.h>
#include <cmath>
#include <std_msgs/String.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <image_transport/image_transport.h>
#include <unistd.h>
using std::string;
using std::stringstream;
using std::cout;
using namespace cv;
void img_put(string s,const Mat &cvi){
	namedWindow(s.c_str(),0);
	resizeWindow(s.c_str(),640,480);
	imshow(s.c_str(),cvi);
}

const int maxn=2000;
//假设图像行和列的像素上限是2000
int queue[maxn],tot=0;//用来记录行分割的位置
int q1[maxn],t1=0;//初次列分割位置 
int q2[maxn],t2=0;//重新列分割文职
int arr[maxn][maxn];//记录对应位置的黑白像素
int Sum[maxn][maxn];//二维前缀和
int ask(int x1,int y1,int x2,int y2){ //询问二维区间像素数量
	return Sum[x2][y2]-Sum[x2][y1-1]-Sum[x1-1][y2]+Sum[x1-1][y1-1];
}
const int maxm=10000;
const int maxt=36;
//假设匹配的汉字数上限是10000,汉字图像大小是36
long long val[maxm][maxt];//匹配值
char hanzi[maxm][10];//汉字词
int cnt=-1;//从txt中读入的汉字数量

std::ifstream kin;
void redirect_fin(string s)
{
	kin.close();
	if(s.empty())return;
	kin.open(&s[0]);
}
int getcnt(long long x){//对于一个值，求出他二进制里有多少个一
	int ct=0;
	while(x){
		ct++;
		x-=(x&(-x));//lowbit
	}
	return ct;
}
int minloc,mindis;
void check(const Mat &src,int l,int r,int u,int d){
	//cout<<u<<' '<<d<<' '<<l<<' '<<r<<'\n';
	//Rect rect(u,l,d-u+1,r-l+1);
	Rect rect(l,u,r-l+1,d-u+1);
	Mat resized=src(rect);
	int len=36;
	resize(resized,resized,Size(len,len));
	
	long long cut[36];//被裁出来的汉字储存在这  
	for(int i=0;i<len;i++){				//width
		cut[i]=0;
		for(int j=0;j<len;j++){			//height
			int val=resized.ptr<uchar>(i)[j];
			if(val<114)cut[i]|=(1ll<<j);//是黑色，算入
		}
	}
	minloc=-1,mindis=114514;	
	for(int i=0;i<cnt;i++) {
		int res=0;
		for(int j=0;j<len;j++){
			long long x=cut[j]^val[i][j];
			//cout<<i<<' '<<j<<' '<<cut[j]<<' '<<val[i][j]<<'\n';
			res+=getcnt(x);
			if(res>mindis)break;//剪枝优化
		}
		if(res<mindis){
			minloc=i;
			mindis=res;
		}
		//cout<<i<<' '<<res<<'\n';
	}
	//cout<<minloc<<' '<<mindis<<'\n';
	//img_put("test",resized);
	//waitKey(0);
}
string recognition(const Mat &src,Mat &res){
	
	char cwd[1000];
	getcwd(cwd,sizeof(cwd));
	string path=cwd;
	string path1=path+"/match/table.txt";
	redirect_fin(path1);
	while(kin>>hanzi[++cnt]){
		//ROS_INFO("%s",hanzi[cnt]);
	}
	//cout<<path1<<' '<<cnt<<'\n';
	path1=path+"/match/match.txt";
	redirect_fin(path1);
	for(int i=0;i<cnt;i++){
		for(int j=0;j<maxt;j++){
			kin>>val[i][j];
		}
	}

	int width=src.rows,height=src.cols;
	//cout<<width<<' '<<height<<'\n';
	Mat gray,Threshold;
	cvtColor(src,gray,COLOR_BGR2GRAY);//转化成灰度图
	threshold(gray, Threshold,250, 255, THRESH_BINARY);//然后二值化
	
	Mat temp1,temp2;//处理过程中行分割和列分割的具体情况
	cvtColor(src,temp1,COLOR_BGR2GRAY);//转化成灰度图
	cvtColor(src,temp2,COLOR_BGR2GRAY);//转化成灰度图
	for(int i=1;i<=width;i++){//将二值化的数值用数组记录起来
		for(int j=1;j<=height;j++){
			int val=Threshold.ptr<uchar>(i-1)[j-1];
			if(val>250) arr[i][j]=0;//白色记为0
			else arr[i][j]=1; //黑色记为1
			Sum[i][j]=Sum[i-1][j]+Sum[i][j-1]-Sum[i-1][j-1]+arr[i][j];
			temp1.ptr<uchar>(i-1)[j-1]=arr[i][j]*255;
			temp2.ptr<uchar>(i-1)[j-1]=arr[i][j]*255;
		}
	}

	//行分割
	int tot=0;
	int match=1;//标记是行开始还是行结束
	for(int i=1;i<=width;i++){
		int v=ask(i,1,i,height);//询问这行的黑色像素个数,其中数值向下是width，水平向右是height
		if(match==1 and v!=0){//行开始有元素
			match*=-1;
			queue[++tot]=i;//用queue两个一组记录
		}
		else if(match==-1 and v==0 ){//行结束没有元素
			match*=-1;
			queue[++tot]=i;
		}
	}
	for(int v=1;v<=tot;v++){
		int i=queue[v];
		for(int j=1;j<=height;j++){
			temp1.ptr<uchar>(i-1)[j-1]=255;
			temp2.ptr<uchar>(i-1)[j-1]=255;
		}
	}

	//列分割
	string s;
	for(int t=1;t<=tot;t+=2){
		t1=0,t2=0;
		int i=queue[t],j=queue[t+1];
		int match=1;
		for(int k=1;k<=height;k++){
			int v=ask(i,k,j,k);//询问第k列从第i行到第j行的黑色元素
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
			int x=i,y=j;//x是行首，y是行尾,q[k-1]是列首，q[k]是列尾   
			
			//得出当前分割的情况
			int nowl=q1[k-1],nowr=q1[k];
			while(ask(x,nowl,x,nowr)==0)x++;
			while(ask(y,nowl,y,nowr)==0)y--;
			check(Threshold,nowl-1,nowr-1,x-1,y-1);

			nowloc=minloc;
			nowdis=mindis;
			while(k+2<=t1){
				int l=q1[k+1],r=q1[k+2];
				if((r+l)/2-(nowr+nowl)/2>j-i)break;//重心的距离应该不比行高度还远
				x=i,y=j;
				while(ask(x,nowl,x,r)==0)x++;
				while(ask(y,nowl,y,r)==0)y--;
				check(Threshold,nowl-1,r-1,x-1,y-1);
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
			s+=hanzi[nowloc];
		}
		
		//反色输出
		for(int k=1;k<=t2;k+=2){//两个一组记录列位置
			for(int v=i;v<j;v++){
				temp2.ptr<uchar>(v-1)[q2[k]-1]=255;
				temp2.ptr<uchar>(v-1)[q2[k+1]-1]=255;
			}
		}
	}
	img_put("原图像",src);
	img_put("二值化",Threshold);
	img_put("行分割",temp1);
	img_put("列分割",temp2);
	ROS_INFO("得到的结果是：\n%s\n对图片按下任意键继续",s.c_str());
	waitKey(0);
	return s;
}

void slp(){
	ros::Rate rate = 1;
	ros::spinOnce();
	rate.sleep();
}

int main(int argc, char **argv){
	setlocale(LC_ALL,"zh_CN.UTF-8");
	ros::init(argc,argv,"opencv");
	ros::NodeHandle n;//节点句柄
	string name="1.png";
	ROS_INFO("%d",argc);
	if(argc==2){
		name=argv[1];
		cout<<name<<'\n';
	}
	else{
		ROS_INFO("传入的参数不是1个，将使用默认参数");
	}
	Mat source=imread(name),result;
	if(source.empty()){
		ROS_ERROR("读取失败,请检查图片名字");
		ros::spin();
	}
	string s=recognition(source,result);
	ros::Publisher pub = n.advertise<std_msgs::String>("beikuzi",1000);
	std_msgs::String ss;
	ss.data=s.c_str();
	while(1){
		pub.publish(ss);
		ROS_INFO("已发送");
		slp();
	}
	return 0;	
}
