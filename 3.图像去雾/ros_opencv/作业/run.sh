source devel/setup.bash
echo $ROS_PACKAGE_PATH
gnome-terminal -- roscore
gnome-terminal --title="opencv" -- rosrun cv opencv