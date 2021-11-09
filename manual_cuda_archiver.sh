#!/bin/bash

compile="/usr/local/cuda-11.0/bin/nvcc \
-c \
-I/home/r.kigaru/slam_robotics/slam_impls/mesh_hashing/src \
-I/home/r.kigaru/slam_robotics/slam_impls/mesh_hashing/src/extern/cuda"

IFS=$'\n'
set -f
for i in $(cat cu_sources+deps.txt); do echo $compile $i; done
