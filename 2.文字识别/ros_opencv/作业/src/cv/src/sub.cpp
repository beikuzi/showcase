#include <sstream>
#include <iostream>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <cstdio>
#include<cstdlib>
#include<vector>
void callback(const std_msgs::String &msg){//回调函数
    static int cnt=0;
	ROS_INFO("收到：%s,%d次",msg.data.c_str(),++cnt);
}
int main(int argc,char **argv){
	setlocale(LC_ALL,"");
	ros::init(argc,argv,"sub");
	ros::NodeHandle n;
	ros::Subscriber sub=n.subscribe("beikuzi",1000,callback);
	ros::spin();
	return 0;
}