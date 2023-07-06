cmake_minimum_required(VERSION 3.2)
project (ImitatorNPA)

set(SOURCE_EXE main.cpp speed.cpp course.cpp position.cpp psiTeta.cpp deviation.cpp Utilities/radianControl.cpp Experiments/etalonNS.cpp)

include_directories(".Headers/")

add_executable(ImitatorNPA ${SOURCE_EXE})
