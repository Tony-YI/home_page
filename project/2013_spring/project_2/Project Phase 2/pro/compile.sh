# CSC3420 Project Phase 2
# Single-cycle simulator
#
# This is a script file for you to compile and run the simulator
# During project marking, the compilation statement will be exactly the same with this file
#
# If you cannot execute this file, please change the premission to 744 by command:
# chmod u+x compile.sh
#
# Usage:
# ./compile.sh machine_code.asc

gcc -o simulator simulator.c phase2.c -Wall
./simulator $1
