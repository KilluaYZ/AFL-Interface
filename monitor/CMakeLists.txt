cmake_minimum_required(VERSION 3.10)
project(AFL_Monitor)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES
        src/controller/TestController.hpp
        src/dto/MsgDto.hpp
        src/AppComponent.hpp
        src/App.cpp
        src/utils/case_info.hpp
        src/utils/json.hpp
        src/controller/Controller.hpp
        src/utils/case_info.cpp
        src/FuzzerManager/Fuzzer.hpp
        src/FuzzerManager/Fuzzer.cpp
        src/FuzzerManager/FuzzerManager.hpp
        src/FuzzerManager/FuzzerManager.cpp
)
#find_package(nolhmann_json REQUIRED)
find_package(oatpp REQUIRED)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} oatpp::oatpp)
