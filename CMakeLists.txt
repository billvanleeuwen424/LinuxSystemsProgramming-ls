cmake_minimum_required(VERSION 3.18.0)
project(ls VERSION 0.1.0)


add_executable(ls main.c)

include_directories(/home/billslaptop/Documents/TrentU/3380/lab2all/lab2/list)
find_library(list liblist.a PATHS /home/billslaptop/Documents/TrentU/3380/lab2all/lab2/list/build)


target_link_libraries(ls ${list})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


