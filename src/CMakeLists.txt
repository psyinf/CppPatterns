cmake_minimum_required(VERSION 2.8)
project (Patterns)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

ADD_SUBDIRECTORY(Strategy)
ADD_SUBDIRECTORY(Active)