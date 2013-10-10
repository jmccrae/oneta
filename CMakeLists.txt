cmake_minimum_required (VERSION 2.6)
project (oneta)
link_libraries(arpack)
link_libraries(gsl)
make_directory(bin)
add_executable(bin/mate-finding src/mate-finding.cpp)
add_executable(bin/sim-csv src/sim-csv.cpp)
add_executable(bin/lsi src/lsi.cpp)
add_executable(bin/oneta src/oneta-gsl.cpp)
add_executable(bin/lda src/lda.cpp)
add_executable(bin/translator-train src/translator-train.cpp)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-O2 --std=c++0x -Wall")        ## Optimize
    set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()
