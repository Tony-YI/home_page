This directory contains the implementation on the intel board
E.g


1. incoming data handler
2. the opencv object detection.
3. control signal according to the object detection 
4. outgoing data controller

Currently the robot has the following state

1. ROBOT_INIT
This state will chech whether all the system are online
	->connect to cell phone WiFi
	->test the cell phone camera functionality
	->check connection with the car (or mbed)
	->test the motion system capability
	->goto ROBOT_READY

2. ROBOT_READY
In this state, the robot will be waiting for user's command to begin working
	->take a hello world picture
	->wait for user command to continue

3. ROBOT_CHECK_TARGET
In this state, the robot will look around and try to find the target
	->check whether the target is inside the scope
	->if so, goto ROBOT_FOUND_TARGET
	->if no, stay in this state
4. ROBOT_FOUND_TARGET
5. ROBOT_ANALYZE_IMAGE
6. ROBOT_APPROACH_REF

