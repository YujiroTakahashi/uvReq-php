cmake_minimum_required(VERSION 3.5.1)

project(uvrequest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 -DPHP_ATOM_INC -DHAVE_CONFIG_H -pthread -funroll-loops")
set(CMAKE_CXX_FLAGS "-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 -DPHP_ATOM_INC -DHAVE_CONFIG_H -funroll-loops")

execute_process(COMMAND "php-config" "--prefix" OUTPUT_VARIABLE PHP_BASE_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "php-config" "--include-dir" OUTPUT_VARIABLE PHP_INCLUDE_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "php-config" "--extension-dir" OUTPUT_VARIABLE EXTENSION_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

set(COMPILE_DL_UVREQUEST 1)
include(config/config.cmake)

find_package(CURL)
find_package(uv CONFIG REQUIRED)


include_directories(
    "${PHP_INCLUDE_PATH}"
    "${PHP_INCLUDE_PATH}/main"
    "${PHP_INCLUDE_PATH}/TSRM"
    "${PHP_INCLUDE_PATH}/Zend"
    "${PHP_INCLUDE_PATH}/ext"
    "${PHP_INCLUDE_PATH}/ext/date/lib"
    "${PHP_INCLUDE_PATH}/sapi"
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR})


add_library(uvrequest-shared SHARED 
    ${CMAKE_SOURCE_DIR}/classes/uvreq.cc
    ${CMAKE_SOURCE_DIR}/src/uvrequest.c)

target_link_libraries(uvrequest-shared
    ${CURL_LIBRARIES}
    ${uv_LIBRARIES})

set_target_properties(uvrequest-shared PROPERTIES LINK_FLAGS "-Wl,-rpath,${PHP_BASE_PATH}/lib")
set_target_properties(uvrequest-shared PROPERTIES PREFIX "" OUTPUT_NAME uvrequest) 

install (TARGETS uvrequest-shared DESTINATION ${EXTENSION_DIR} 
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_WRITE GROUP_READ 
                WORLD_EXECUTE WORLD_READ)
