cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PasswordGuessingCurves)

# global settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# gurobi c++ package
if(APPLE)
    set(GUROBI_PATH_DEFAULT "/Library/gurobi1201/macos_universal2")
elseif(WIN32)
    set(GUROBI_PATH_DEFAULT "C:/gurobi1201/win64")
else()
    set(GUROBI_PATH_DEFAULT "/opt/gurobi1201/linux64")
endif()
set(GUROBI_PATH ${GUROBI_PATH_DEFAULT} CACHE PATH "path to gurobi installation")
set(GUROBI_CPPLIB -L${GUROBI_PATH}/lib -lgurobi_c++ -lgurobi110 -lm)

# test executable
# add_executable(test test.cpp ${SRC_FILES})
# target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(test PUBLIC ${GUROBI_PATH}/include)
# target_link_libraries(test ${GUROBI_CPPLIB})

# example executable
add_executable(example examples/example.cpp ${SRC_FILES})
target_include_directories(example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(example PUBLIC ${GUROBI_PATH}/include)
target_link_libraries(example ${GUROBI_CPPLIB})

# interactive executable
add_executable(interactive interactive.cpp ${SRC_FILES})
target_include_directories(interactive PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(interactive PUBLIC ${GUROBI_PATH}/include)
target_link_libraries(interactive ${GUROBI_CPPLIB})


# statistics 
# add_executable(statistics dataset_stats/stats.cpp ${SRC_FILES})
# target_include_directories(statistics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(statistics PUBLIC ${GUROBI_PATH}/include)
# target_link_libraries(statistics ${GUROBI_CPPLIB})

# threshold -- my stuff 
add_executable(threshold thresholds/threshold.cpp ${SRC_FILES})
target_include_directories(threshold PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(threshold PUBLIC ${GUROBI_PATH}/include)
target_link_libraries(threshold ${GUROBI_CPPLIB})


# example of adding your own executable
# add_executable(<executable_name> <source_file_name>.cpp ${SRC_FILES})
# target_include_directories(<executable_name> PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(<executable_name> PUBLIC ${GUROBI_PATH}/include)
# target_link_libraries(<executable_name> ${GUROBI_CPPLIB})
