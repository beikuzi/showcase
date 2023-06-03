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
void tfs_set2(geometry_msgs::TransformStamped &ts,int seq,string id,string cid,
	double x,double y,double z,double X,double Y,double Z,double W
){
	ts.header.seq=seq;
	ts.header.stamp=ros::Time::now();
	ts.header.frame_id=id;
	ts.child_frame_id=cid;
	ts.transform.translation.x=x;
	ts.transform.translation.y=y;
	ts.transform.translation.z=z;
	ts.transform.rotation.x=X;
	ts.transform.rotation.y=Y;
	ts.transform.rotation.z=Z;
	ts.transform.rotation.w=W;
}
void callback(const gazebo_msgs::ModelStates::ConstPtr& msg){
	static int car1=0,car2=0,car3;//名字为my_car（作为键盘操作那辆车）的索引
	static int flag=0;
	if(flag==0)
	for (int i=0;i< msg->name.size();i++) {
        if (msg->name[i]=="mycar"){
            car1=i;
        }
		else if(msg->name[i]=="mycar2"){
			car2=i;
		}
		else if(msg->name[i]=="mycar3"){
			car3=i;
		}
		cout<<msg->name[i]<<'\n';
    }flag=1;

    geometry_msgs::Pose pos1=msg->pose[car1],pos2=msg->pose[car2],pos3=msg->pose[car3];
	geometry_msgs::Twist twmsg;
	static tf2_ros::TransformBroadcaster bdc1,bdc2,bdc3;//变换广播
	geometry_msgs::TransformStamped son1,son2,son3;//坐标系变换
	string name="ground",name1="car1",name2="car2",name3="car3";
	tfs_set2(son1,1,name,name1,pos1.position.x,pos1.position.y,pos1.position.z,
		pos1.orientation.x,pos1.orientation.y,pos1.orientation.z,pos1.orientation.w);
	bdc1.sendTransform(son1);
	tfs_set2(son2,2,name,name2,pos2.position.x-1.5,pos2.position.y-1.5,pos2.position.z,
		pos2.orientation.x,pos2.orientation.y,pos2.orientation.z,pos2.orientation.w);
	bdc2.sendTransform(son2);
	tfs_set2(son3,3,name,name3,pos3.position.x-1.5,pos3.position.y+1.5,pos3.position.z,
		pos3.orientation.x,pos3.orientation.y,pos3.orientation.z,pos3.orientation.w);
	bdc3.sendTransform(son3);
	ROS_INFO("已发布");
	
}
void fun(){
	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("/gazebo/model_states", 100, callback);
	ros::spin();
}
int main(int argc,char **argv){
	setlocale(LC_ALL,"");
	ros::init(argc,argv,"pub");
	
	fun();
	ros::spin();
	return 0;
}
