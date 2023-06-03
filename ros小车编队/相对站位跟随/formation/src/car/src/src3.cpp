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
#include<tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <gazebo_msgs/ModelStates.h>
using std::cout;
using std::string;

void pts_set(geometry_msgs::PointStamped &pts,string id,
	double x,double y,double z
){
	pts.header.frame_id=id;
	pts.header.stamp=ros::Time();
	pts.point.x=x;
	pts.point.y=y;
	pts.point.z=z;
}
void slp(){
	ros::Rate rate = 100;
	ros::spinOnce();
	rate.sleep();
}
string name1="car1";
void fun(){
	ros::NodeHandle n;
	
	
	tf2_ros::Buffer bf;
	tf2_ros::TransformListener lsner(bf);
	int cnt=0;
	ros::Publisher pub=n.advertise<geometry_msgs::Twist>("cmd_topic2", 1000);
	ros::Publisher pub2=n.advertise<geometry_msgs::Twist>("cmd_topic3", 1000);
	while(ros::ok()){
		try{
			geometry_msgs::PointStamped pts1,pts2;//被检测到的物体的坐标
			pts_set(pts1,"carA",1,1,0);//希望第二辆车在car1的1.5，1.5处
			pts_set(pts2,"carA",0,-1.5,0);
			geometry_msgs::PointStamped ptb1,ptb2;//直接转化成和地面相比较
			ptb1=bf.transform(pts1,"carB");
			ptb2=bf.transform(pts2,"carC");

			geometry_msgs::Twist twist;
            twist.linear.x = 1*sqrt(pow(ptb1.point.x,2) + pow(ptb1.point.y,2));
            twist.angular.z = 4*atan2(ptb1.point.y,ptb1.point.x);
            pub.publish(twist);
			geometry_msgs::Twist twist2;
            twist2.linear.x = 1*sqrt(pow(ptb2.point.x,2) + pow(ptb2.point.y,2));
            twist2.angular.z = 4*atan2(ptb2.point.y,ptb2.point.x);
            pub2.publish(twist2);
		}
		catch(const std::exception &e){
			//ROS_INFO("异常：%s",e.what());
		}
		slp();
	}
}
int main(int argc,char **argv){
	setlocale(LC_ALL,"");
	ros::init(argc,argv,"sub");
	
	fun();
	ros::spin();
	return 0;
}
