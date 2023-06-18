source devel/setup.bash
echo $ROS_PACKAGE_PATH

gnome-terminal -- roslaunch formation 1.launch
#gnome-terminal -- rviz -d "$(find src/pkg3)/urdf/1.rviz"
sleep 5
gnome-terminal --title="key_input" -- rosrun formation key 
gnome-terminal --title="boardcast" -- rosrun formation pub
gnome-terminal --title="listener" -- rosrun formation sub

