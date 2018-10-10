cmake_minimum_required(VERSION 2.8)
project (Patterns)
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else()
# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
endif()


ADD_SUBDIRECTORY(Strategy)
ADD_SUBDIRECTORY(Active)
ADD_SUBDIRECTORY(Observer)