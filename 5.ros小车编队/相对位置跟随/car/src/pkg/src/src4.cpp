#include<sstream>
#include<iostream>
#include<ros/ros.h>
#include<std_msgs/String.h>
#include<geometry_msgs/Twist.h>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<vector>

#include<tf2_ros/static_transform_broadcaster.h>//tf,TransForm Frame,静态坐标变换广播
#include<tf2_ros/transform_broadcaster.h>//动态坐标广播
#include<geometry_msgs/TransformStamped.h>
#include<tf2/LinearMath/Quaternion.h>//四元数

#include<geometry_msgs/PointStamped.h>//坐标点数据
#include<tf2_ros/transform_listener.h>//接收

#include <gazebo_msgs/ModelStates.h>
using std::cout;


typedef std::string string;
void slp(){
	ros::Rate rate = 100;
	ros::spinOnce();
	rate.sleep();
}
string name1="car1",name2="car2",name3="car3";
void fun(){
	ros::NodeHandle n;
	ros::Publisher pub2=n.advertise<geometry_msgs::Twist>("cmd_topic3", 1000);
	tf2_ros::Buffer bf;
	tf2_ros::TransformListener lsner(bf);
	int cnt=0;
	while(ros::ok()){
		try{
			geometry_msgs::TransformStamped tfs2;
			tfs2=bf.lookupTransform(name3,name1,ros::Time::now());
			geometry_msgs::Twist twist2;
			twist2.linear.x = sqrt(pow(tfs2.transform.translation.x,2) + pow(tfs2.transform.translation.y,2));
            twist2.angular.z = 4*atan2(tfs2.transform.translation.y,tfs2.transform.translation.x);
			pub2.publish(twist2);
			ROS_INFO("%d %lf %lf",cnt++,tfs2.transform.translation.x,tfs2.transform.translation.y);
		}
		catch(const std::exception &e){
			//ROS_INFO("异常：%s",e.what());
		}
		slp();
	}
}
int main(int argc,char **argv){
	setlocale(LC_ALL,"");
	ros::init(argc,argv,"sub2");
	
	fun();
	ros::spin();
	return 0;
}
