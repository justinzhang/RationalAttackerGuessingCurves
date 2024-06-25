cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PasswordGuessingCurves)

# global settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# gurobi c++ package
# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# find_package(GUROBI REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

# test executable
add_executable(test test.cpp ${SRC_FILES})
target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# FindGUROBI.cmake not working, hardcode instead, fix later

# target_include_directories(test ${GUROBI_INCLUDE_DIRS})
# target_link_libraries(test ${GUROBI_CXX_LIBRARY})
# target_link_libraries(test ${GUROBI_LIBRARY})

set(GUROBI_PATH /Library/gurobi1102/macos_universal2)
set(GUROBI_CPPLIB -L${GUROBI_PATH}/lib -lgurobi_c++ -lgurobi110 -lm)
target_include_directories(test PUBLIC ${GUROBI_PATH}/include)
target_link_libraries(test ${GUROBI_CPPLIB})




# main executable
add_executable(main main.cpp ${SRC_FILES})
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(GUROBI_PATH /Library/gurobi1102/macos_universal2)
set(GUROBI_CPPLIB -L${GUROBI_PATH}/lib -lgurobi_c++ -lgurobi110 -lm)
target_include_directories(main PUBLIC ${GUROBI_PATH}/include)
target_link_libraries(main ${GUROBI_CPPLIB})

