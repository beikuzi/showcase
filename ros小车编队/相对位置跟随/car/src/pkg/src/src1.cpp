#include <sstream>
#include <iostream>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>
#include <termios.h>
#include <cstdio>
#include<cstdlib>
#include<vector>
#include<tf2_ros/static_transform_broadcaster.h>//tf,TransForm Frame,静态坐标变换广播
#include<tf2_ros/transform_broadcaster.h>//动态坐标广播
#include<geometry_msgs/TransformStamped.h>
#include<tf2/LinearMath/Quaternion.h>//四元数
using std::cout;


//using namespace ros;
//using namespace std;

typedef std::string string;
typedef std::stringstream stringstream;
void slp(){
	ros::Rate rate = 100;
	ros::spinOnce();
	rate.sleep();
}
#define KEYCODE_UP    0x41
#define KEYCODE_DOWN  0x42
#define KEYCODE_LEFT  0x44
#define KEYCODE_RIGHT 0x43
void fun(){
	ros::NodeHandle n;
	struct termios cooked, raw;
	ros::Publisher pub = n.advertise<geometry_msgs::Twist>("cmd_topic1", 1000);
	static int kfd=0;
	geometry_msgs::Twist msg,last_msg;
	char c;

	tcgetattr(kfd, &cooked);
	memcpy(&raw, &cooked, sizeof(struct termios));
	raw.c_lflag &= ~(ICANON|ECHO);
	raw.c_cc[VEOL] = 1;
	raw.c_cc[VEOF] = 2;
	//raw.c_cc[VMIN] = 0;
	//raw.c_cc[VTIME] = 1;
	tcsetattr(kfd, TCSANOW, &raw);
	ROS_INFO("请使用方向上下左右键");
	int flag=0;
	while(1){
		c=0;
		c=getchar();
		if(c==27){
			getchar();
			c=getchar();
		}
		cout<<c<<'\n';	
		switch(c){
			case KEYCODE_UP :
			case 'w':
				ROS_INFO("up");
				msg.linear.x=1;
				msg.angular.z=0;
				flag=1;
				break;
			case KEYCODE_DOWN :
			case 's':
				ROS_INFO("down");
				msg.linear.x=-1;
				msg.angular.z=0;
				flag=1;
				break;
			case KEYCODE_LEFT:
			case 'a':
				ROS_INFO("left");
				msg.linear.x=0;
				msg.angular.z=1;
				flag=1;
				break;
			case KEYCODE_RIGHT:
			case 'd':
				ROS_INFO("right");
				msg.linear.x=0;
				msg.angular.z=-1;
				flag=1;
				break;
			default:
				ROS_INFO("stop");
				flag=0;
				break;
		}
		if(flag==0){
			msg.linear.x=0;
			msg.linear.y=0;
			msg.linear.z=0;
			msg.angular.z=0;
			msg.angular.y=0;
			msg.angular.x=0;
		}
		if(msg!=last_msg)pub.publish(msg);//让消息不重复发送
		last_msg=msg;
		slp();
	}
}
int main(int argc, char **argv){
	setlocale(LC_ALL,"");
	ros::init(argc,argv,"key");
	fun();
	ros::spin();
	return 0;	
}
