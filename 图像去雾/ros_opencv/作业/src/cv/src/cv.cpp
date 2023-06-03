#include <sstream>
#include <iostream>
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
using std::string;
using std::stringstream;
using namespace cv;
void img_put(string s,const Mat &cvi){
	namedWindow(s.c_str(),0);
	resizeWindow(s.c_str(),640,480);
	imshow(s.c_str(),cvi);
}
void cap(){
	VideoCapture cap;  
	cap.set(CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CAP_PROP_FRAME_HEIGHT, 480);
	cap.set(CAP_PROP_FPS,30);
	cap=0;

	if(!cap.isOpened()){
		ROS_INFO("摄像头不能打开");
		ros::spin();
	}
	Mat img;
	while(1){
		cap.read(img);
		if(img.empty())break;
		img_put("cam",img);
		if(waitKey(33)==32)return;
	}
}
void canny(const Mat &src,Mat &res){
	Mat gray;
	cvtColor(src,gray,COLOR_BGR2GRAY);
	blur(gray,res,Size(3,3));
	Canny(res,res,3,9,3);
	img_put("原图",src);
	img_put("canny边缘检测",res);
	waitKey(0);
}
void sobel(const Mat &src,Mat &res){
	Mat grad_x,grad_y,abs_grad_x,abs_grad_y;
	Sobel(src,grad_x,CV_16S,1,0,3,1,1,BORDER_DEFAULT);
	Sobel(src,grad_y,CV_16S,0,1,3,1,1,BORDER_DEFAULT);
	convertScaleAbs(grad_x,abs_grad_x);
	convertScaleAbs(grad_y,abs_grad_y);
	addWeighted(abs_grad_x,0.5,abs_grad_y,0.5,0,res);
	//img_put("x方向soble",abs_grad_x);
	//img_put("y方向soble",abs_grad_y);
	img_put("原图",src);
	img_put("sobel边缘检测",res);
	waitKey(0);
}
void laplacian(const Mat &src,Mat &res){
	Mat temp,gray;
	GaussianBlur(src,temp,Size(3,3),0,0,BORDER_DEFAULT);
	cvtColor(temp,gray,COLOR_RGB2GRAY);
	Laplacian(gray,res,CV_16S,3,1,0,BORDER_DEFAULT);
	convertScaleAbs(res,res);
	img_put("原图",src);
	img_put("laplacian边缘检测",res);
	waitKey(0);
}
void equalization(const Mat &src,Mat &res){
	int sum[256][3];//前缀和三通道数量
	int T[256][3];//变换结果
	for(int i=0;i<256;i++){//初始化
		sum[i][0]=sum[i][1]=sum[i][2]=0;
	}
	int width=src.rows,height=src.cols;
	std::vector<Mat>channels;
	split(src,channels);
	for(int i=0;i<width;i++){//将整张图片，三个通道不同灰度数量统计起来
		for(int j=0;j<height;j++){
			sum[channels[0].ptr<uchar>(i)[j]][0]++;
			sum[channels[1].ptr<uchar>(i)[j]][1]++;
			sum[channels[2].ptr<uchar>(i)[j]][2]++;
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
	for(int i=0;i<width;i++){//写入新灰度
		for(int j=0;j<height;j++){
			channels[0].ptr<uchar>(i)[j]=T[channels[0].ptr<uchar>(i)[j]][0];
			channels[1].ptr<uchar>(i)[j]=T[channels[1].ptr<uchar>(i)[j]][1];
			channels[2].ptr<uchar>(i)[j]=T[channels[2].ptr<uchar>(i)[j]][2];
		}
	}
	merge(channels,res);
	img_put("原图",src);
	img_put("直方图均衡化",res);
	waitKey(0);
}

int low[2000][2000];//最低值
int temp[2000][2000];
void darkChannel(const Mat &src,Mat &res){
	int width=src.rows,height=src.cols;
	std::vector<Mat>channels;
	split(src,channels);
	Mat dark(width,height,CV_8UC1);

	int A=0;
	int r=3,d=2*r+1;
	double t0=0.3;
	double w=0.95;
			
	for(int j=0;j<height+r;j++){
		for(int i=0;i<width+r;i++){
			if(j>=height || i>=width){
				low[j][i]=255;//超出部分设为最大值
			}
			else{
				int u=channels[0].ptr<uchar>(i)[j];
				int v=channels[1].ptr<uchar>(i)[j];
				int w=channels[2].ptr<uchar>(i)[j];
				A=std::max( max(A,u),max(v,w) );//取全图像最亮的点作为大气层值A 
				int res=std::min( min(u,v) ,w );//取三个通道中最暗的那个
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
	//此时的low是以(i,j)为右下角，直径为d的矩阵中的最小值，而暗通道以中间为中心，所以dark的中心(i,j)等于low的(i+r,j+r)
	for(int j=0;j<height;j++){
		for(int i=0;i<width;i++){
			dark.ptr<uchar>(i)[j]=low[j+r][i+r];
		}
	}
		
	for(int j=0;j<height;j++){
		for(int i=0;i<width;i++){
			double res=dark.ptr<uchar>(i)[j]; //暗通道I(x)
			double temp=1.0-w*res/A;
			
			int r=1.0*(channels[0].ptr<uchar>(i)[j]-A)/((temp>t0)?temp:t0)+A+0.5;
			int g=1.0*(channels[1].ptr<uchar>(i)[j]-A)/((temp>t0)?temp:t0)+A+0.5;
			int b=1.0*(channels[2].ptr<uchar>(i)[j]-A)/((temp>t0)?temp:t0)+A+0.5; 

			channels[0].ptr<uchar>(i)[j]=r;
			channels[1].ptr<uchar>(i)[j]=g;
			channels[2].ptr<uchar>(i)[j]=b;
		}
	}
	merge(channels,res);
	img_put("原图",src);
	img_put("暗通道图像",dark);
	img_put("暗通道先验去雾",res);
	waitKey(0);
}
int main(int argc, char **argv){
	
	setlocale(LC_ALL,"");
	ros::init(argc,argv,"opencv");
	ros::NodeHandle n;
	string name="1.png";
	Mat source=imread(name),result;
	if(source.empty()){
		ROS_ERROR("读取失败,请检查名字");
		ros::spin();
	}
	sobel(source,result);
	canny(source,result);
	laplacian(source,result);

	string name2="2.png";//可以使用3.png，比较明显
	Mat source2=imread(name2),result2;
	if(source2.empty()){
		ROS_ERROR("读取失败,请检查名字");
		ros::spin();
	}
	equalization(source2,result2); 

	string name3="4.png";//也可以看5.png
	Mat source3=imread(name3),result3;
	if(source3.empty()){
		ROS_ERROR("读取失败,请检查名字");
		ros::spin();
	}
	darkChannel(source3,result3);
	return 0;	
}
