source devel/setup.bash
echo $ROS_PACKAGE_PATH

#gnome-terminal -- roscore
#gnome-terminal -- rosrun gazebo_ros spawn_model -urdf -file $(rospack find pkg)/urdf/1.urdf -model mycar
#gnome-terminal -- rosrun gazebo_ros spawn_model -urdf -file $(rospack find pkg)/urdf/2.urdf -model mycar2

gnome-terminal -- roslaunch pkg 1.launch
#gnome-terminal -- rviz -d "$(find src/pkg3)/urdf/1.rviz"
sleep 5
gnome-terminal --title="key_input" -- rosrun pkg key 
gnome-terminal --title="boardcast" -- rosrun pkg pub
gnome-terminal --title="listener" -- rosrun pkg sub
gnome-terminal --title="listener2" -- rosrun pkg sub2
gnome-terminal --title="rqt_graph" -- rqt_graph

#sudo apt-get install libwebpmux3 
#libwebp-dev libgdal-dev libgazebo11-dev ros-noetic-gazebo-dev 
#sudo aptitude install ros-noetic-gazebo-ros-pkgs
