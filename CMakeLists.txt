cmake_minimum_required(VERSION 3.15)
project(pipelineelement)

set(CMAKE_CXX_STANDARD 17)


find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
find_package(spdlog REQUIRED)
find_package(Threads)

find_library(
        SimpleAmqpClient NAMES libSimpleAmqpClient libSimpleAmqpClient.so
        REQUIRED
        HINTS "${CMAKE_PREFIX_PATH}/libs" "./libs/SimpleAmqpClient" "/usr/local/include" "/usr/local/lib64" "/usr/local/lib" "/usr/lib"
)

include_directories(SYSTEM)
message(STATUS "System directory \"${SYSTEM}\"")

include_directories(${LIBMONGOCXX_INCLUDE_DIR})
message(STATUS "LIBMONGOCXX directory ${LIBMONGOCXX_INCLUDE_DIR}")

include_directories(${LIBBSONCXX_INCLUDE_DIR})
message(STATUS "LIBBSONCXX directory ${LIBBSONCXX_INCLUDE_DIR}")

include_directories("./lib")

# standard includes
include_directories("/usr/local/include")
include_directories("/usr/local/include/bsoncxx/v_noabi")
include_directories("/usr/local/include/mongocxx/v_noabi")

# Alpine Linux includes
include_directories("/usr/include")
include_directories("/usr/include/bsoncxx/v_noabi")
include_directories("/usr/include/mongocxx/v_noabi")

# optional includes for MacPorts compatibility
include_directories("/opt/local/include")
include_directories("/opt/local/include/bsoncxx/v_noabi")
include_directories("/opt/local/include/mongocxx/v_noabi")

add_executable(pipelineelement src/main.cpp src/PipelineSubscriber.cpp)

message(STATUS "SimpleAmqpClient path \"${SimpleAmqpClient}\"")

target_link_libraries(pipelineelement ${LIBMONGOCXX_LIBRARIES})
target_link_libraries(pipelineelement ${LIBBSONCXX_LIBRARIES})
target_link_libraries(pipelineelement spdlog::spdlog)
target_link_libraries(pipelineelement ${SimpleAmqpClient} )