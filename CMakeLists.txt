cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PasswordGuessingCurves)

# global settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# gurobi c++ package
if(APPLE)
    set(GUROBI_PATH_DEFAULT "/Library/gurobi1102/macos_universal2")
elseif(WIN32)
    set(GUROBI_PATH_DEFAULT "C:/gurobi1102/win64")
else()
    set(GUROBI_PATH_DEFAULT "/opt/gurobi1102/linux64")
endif()
set(GUROBI_PATH ${GUROBI_PATH_DEFAULT} CACHE PATH "path to gurobi installation")
set(GUROBI_CPPLIB -L${GUROBI_PATH}/lib -lgurobi_c++ -lgurobi110 -lm)

# test executable
add_executable(test test.cpp ${SRC_FILES})
target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(test PUBLIC ${GUROBI_PATH}/include)
target_link_libraries(test ${GUROBI_CPPLIB})

# interactive executable
add_executable(interactive interactive.cpp ${SRC_FILES})
target_include_directories(interactive PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(interactive PUBLIC ${GUROBI_PATH}/include)
target_link_libraries(interactive ${GUROBI_CPPLIB})

# example of adding your own executable
# add_executable(<executable_name> <source_file_name>.cpp ${SRC_FILES})
# target_include_directories(<executable_name> PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(<executable_name> PUBLIC ${GUROBI_PATH}/include)
# target_link_libraries(<executable_name> ${GUROBI_CPPLIB})
