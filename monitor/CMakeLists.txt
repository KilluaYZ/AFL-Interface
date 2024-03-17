cmake_minimum_required(VERSION 3.10)
project(AFL_Monitor)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/controller/TestController.hpp src/dto/TestMsgDto.hpp src/AppComponent.hpp src/App.cpp)
find_package(oatpp REQUIRED)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} oatpp::oatpp)
