source devel/setup.bash
echo $ROS_PACKAGE_PATH
gnome-terminal -- roscore
gnome-terminal --title="opencv" -- rosrun cv opencv $1
gnome-terminal --title="sub" -- rosrun cv sub
sleep 1
gnome-terminal --title="rqt_graph" -- rqt_graph