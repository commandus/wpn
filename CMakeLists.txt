project(wpn)
cmake_minimum_required(VERSION 3.4.0)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../)
set(EXEC_CLI "wpn")
file(GLOB SOURCE_CLI *.cpp mcs/*.cpp mcs/gen/android_checkin.pb.cc mcs/gen/checkin.pb.cc mcs/gen/mcs.pb.cc)
file(GLOB HEADERS_CLI *.h mcs/*.h mcs/gen/*.h third_party/*.h third_party/*.hpp)

target_link_libraries(${EXEC_CLI})
target_include_directories(${SO_LNOTIFY} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${HEADERS_CLI} }

install(TARGETS ${PROJECT_NAME} DESTINATION bin)