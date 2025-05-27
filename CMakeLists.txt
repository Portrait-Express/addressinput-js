cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0043 NEW)
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(addressinput-js)

# libaddressinput
set(LIBADDRESS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/libaddressinput/cpp")
include_directories("${LIBADDRESS_DIR}/include" ${CMAKE_JS_INC})
set(LIBADDR_DEBUG "")
if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(LIBADDR_DEBUG "-DDEBUG=yes")
endif()
add_custom_command(
    OUTPUT ${LIBADDRESS_DIR}/out/Default/obj/libaddressinput.a
    COMMAND export GYP_GENERATORS='ninja'
    COMMAND gyp --depth . -Dcomponent=static_library -DENABLE_FPIC=yes ${LIBADDR_DEBUG}
    COMMAND ninja -C out/Default
    WORKING_DIRECTORY ${LIBADDRESS_DIR})
add_custom_target(libaddressinput_build DEPENDS ${LIBADDRESS_DIR}/out/Default/obj/libaddressinput.a)
add_library(libaddressinput STATIC IMPORTED GLOBAL)
add_dependencies(libaddressinput libaddressinput_build)
set_target_properties(libaddressinput PROPERTIES IMPORTED_LOCATION ${LIBADDRESS_DIR}/out/Default/obj/libaddressinput.a)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")

# addressinput-js
file(GLOB JSINPUT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp/*.cc")
add_library(${PROJECT_NAME} SHARED ${JSINPUT_SOURCES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
# Link re2 required by libaddressinput since we compile it statically
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} libaddressinput re2 dl backtrace) 

add_definitions(-DNAPI_VERSION=9)

