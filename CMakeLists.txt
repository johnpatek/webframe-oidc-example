cmake_minimum_required(VERSION 3.15)

if(NOT VCPKG_ROOT)
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
    if(NOT VCPKG_ROOT)
        message(FATAL_ERROR "VCPKG_ROOT environment variable is not set. Please set it to the root of your vcpkg installation.")
    endif()
endif()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(webframe-oidc-example)

find_package(argparse CONFIG REQUIRED)
find_package(unofficial-sqlite3 REQUIRED)
find_package(megamimes CONFIG REQUIRED)
find_package(hyperpage CONFIG REQUIRED)
find_package(webframe CONFIG REQUIRED)
find_path(UNQLITE_INCLUDE_DIRS "unqlite.h" REQUIRED)
find_library(UNQLITE_LIBRARIES unqlite REQUIRED)

include(cmake/hyperpage.cmake)

add_library(oidcapp_base INTERFACE)
target_include_directories(oidcapp_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src ${UNQLITE_INCLUDE_DIRS})
target_link_libraries(oidcapp_base INTERFACE hyperpage::hyperpage ${UNQLITE_LIBRARIES})

file(GLOB OIDCAPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
if(WIN32)
    add_executable(oidcapp_desktop WIN32 ${OIDCAPP_SOURCES})
else()
    add_executable(oidcapp_desktop ${OIDCAPP_SOURCES})
endif()

target_link_libraries(oidcapp_desktop PRIVATE oidcapp_base webframe::desktop)

add_subdirectory(frontend)

if(WIN32)
    set(ARCHIVE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
else()
    set(ARCHIVE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_executable(oidcapp_server ${OIDCAPP_SOURCES})
target_link_libraries(oidcapp_server PRIVATE oidcapp_base webframe::server)

create_hyperpage_archive(
    TARGET pack_server
    DIST_DIR "${CMAKE_SOURCE_DIR}/frontend/apps/server/dist"
    OUTPUT_FILE "${ARCHIVE_OUTPUT_DIR}/server.db"
    DEPENDS build_server
)

create_hyperpage_archive(
    TARGET pack_desktop
    DIST_DIR "${CMAKE_SOURCE_DIR}/frontend/apps/desktop/dist"
    OUTPUT_FILE "${ARCHIVE_OUTPUT_DIR}/desktop.db"
    DEPENDS build_desktop
)