cmake_minimum_required(VERSION 3.5)
PROJECT(mesh-hashing)

#----------
# C++ version setting
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall")

#----------
# Project variable configurations
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

#----------
# Find packages
FIND_PACKAGE(PkgConfig)
LIST(APPEND
        CMAKE_MODULE_PATH
        ${PROJECT_SOURCE_DIR}/cmake_modules)

# CV utilities
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)
# Parallel computation
FIND_PACKAGE(CUDA REQUIRED)

# Log utilities
FIND_PACKAGE(Glog REQUIRED)

#----------
## Found Headers
GET_PROPERTY(included_dirs
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY INCLUDE_DIRECTORIES)
MESSAGE(STATUS "Found include paths:")
foreach(included_dir ${included_dirs})
    MESSAGE(STATUS ${included_dir})
endforeach()

#----------
## Found Libs

SET(CUDA_DEPENDENCIES "")
LIST(APPEND
        CUDA_DEPENDENCIES
        ${CUDA_CUDART_LIBRARY}
        ${CUDA_CUDA_LIBRARY})

MESSAGE(STATUS "Found libraries:")
foreach(library ${CUDA_DEPENDENCIES})
    MESSAGE(STATUS ${library})
endforeach()
foreach(library ${OTHER_DEPENDENCIES})
    MESSAGE(STATUS ${library})
endforeach()
#---------

#----------
SET(VH ${PROJECT_SOURCE_DIR}/src)

INCLUDE_DIRECTORIES(
        ${VH}
        ${VH}/extern/cuda
        ${EIGEN3_INCLUDE_DIR}
        ${GLOG_INCLUDE_DIRS}
        /usr/include/opencv4
        ${CUDA_TOOLKIT_INCLUDE})

#----------
## Building Libraries
### 1. OpenGL util

### 2. CUDA
# Don't know exactly how it should be configured
LIST(APPEND CUDA_NVCC_FLAGS -gencode=arch=compute_50,code=sm_60)
CUDA_ADD_LIBRARY(mesh-hashing-cuda
        ${VH}/core/hash_table.cu
        ${VH}/core/entry_array.cu
        ${VH}/core/block_array.cu
        ${VH}/core/mesh.cu
        ${VH}/core/collect_block_array.cu

        ${VH}/sensor/rgbd_sensor.cu
        ${VH}/sensor/preprocess.cu

        ${VH}/localizing/point_to_psdf.cu
        ${VH}/mapping/allocate.cu
        ${VH}/mapping/update_simple.cu
        ${VH}/mapping/update_bayesian.cu
        ${VH}/mapping/recycle.cu

        ${VH}/optimize/linear_equations.cu
        ${VH}/optimize/primal_dual.cu

        ${VH}/meshing/marching_cubes.cu

        ${VH}/visualization/colorize.cu
        ${VH}/visualization/compact_mesh.cu
        ${VH}/visualization/bounding_box.cu
        ${VH}/visualization/trajectory.cu
        ${VH}/visualization/compress_mesh.cu
        ${VH}/visualization/extract_bounding_box.cu
        ${VH}/visualization/ray_caster.cu)

TARGET_LINK_LIBRARIES(mesh-hashing-cuda
        ${CUDA_DEPENDENCIES}
        ${GLOG_LIBRARIES}
        -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d)

### 3. C++
ADD_LIBRARY(mesh-hashing
        ${VH}/engine/main_engine.cc
        ${VH}/engine/mapping_engine.cc
        ${VH}/engine/logging_engine.cc
        ${VH}/engine/visualizing_engine.cc

        ${VH}/io/config_manager.cc
        ${VH}/io/mesh_writer.cc

        ${VH}/sensor/rgbd_data_provider.cc)
        #${VH}/tool/cpp/debugger.cc)
TARGET_LINK_LIBRARIES(mesh-hashing
        mesh-hashing-cuda)

#----------
### Loop over
MESSAGE(STATUS ${GLOG_LIBRARIES})
ADD_EXECUTABLE(reconstruction src/app/reconstruction.cc)
TARGET_LINK_LIBRARIES(reconstruction
        mesh-hashing
        -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d
        ${GLOG_LIBRARIES})

ADD_EXECUTABLE(slam src/app/slam.cc)
TARGET_LINK_LIBRARIES(slam
        mesh-hashing
        -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d
        ${GLOG_LIBRARIES})

ADD_EXECUTABLE(block_analysis src/app/block_analysis.cc)
TARGET_LINK_LIBRARIES(block_analysis
        mesh-hashing
        -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d
        ${GLOG_LIBRARIES})

### An ORB app
#OPTION(WITH_ORBSLAM2 "Build with orb slam" ON)
#if (WITH_ORBSLAM2)
#    SET(ORB_SLAM2_PATH ${VH}/extern/orb_slam2)
#    MESSAGE(STATUS "Build with ORB-SLAM2")
#
#    FIND_PACKAGE(Pangolin REQUIRED)
#    FIND_PACKAGE(Eigen3 REQUIRED)
#    INCLUDE_DIRECTORIES(
#            ${ORB_SLAM2_PATH}
#            ${ORB_SLAM2_PATH}/include
#            ${EIGEN3_INCLUDE_DIR})
#    LINK_DIRECTORIES(${ORB_SLAM2_PATH}/lib)
#
#    ADD_EXECUTABLE(orb_slam2 src/app/orb_slam2.cc)
#    TARGET_LINK_LIBRARIES(orb_slam2
#            mesh-hashing
#            ORB_SLAM2
#            ${OpenCV_LIBS}
#            ${Pangolin_LIBRARIES}
#            ${GLOG_LIBRARIES})
#
#    SET_TARGET_PROPERTIES(orb_slam2
#            PROPERTIES
#            COMPILE_DEFINITIONS USE_CUDA_GL)
#endif(WITH_ORBSLAM2)
