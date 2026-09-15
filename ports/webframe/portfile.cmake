vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO maxtek6/WebFrame
    REF c1eab18d1bfee921510be7b23af79222da5fe6e4
    SHA512 c4865cbb25b9fb36f6e464986bdf7717365d58aea198bda615c8df437fd659cb09263eea5f856417768519a2d75914f21c303bc2d26c6956daf40a539a15897e
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_RUNTIME=ON
        -DBUILD_TESTING=OFF
        -DBUILD_EXAMPLE=OFF
        -DBUILD_DOXYGEN=OFF
        -DVCPKG_ROOT=${VCPKG_ROOT_DIR}
)

vcpkg_cmake_build()

# Install headers
file(INSTALL "${SOURCE_PATH}/include/"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include")

# Install release libraries
if(NOT VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL "release")
    file(GLOB WEBFRAME_REL_LIBS
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/src/*.lib"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/src/*.a"
    )
    if(WEBFRAME_REL_LIBS)
        file(INSTALL ${WEBFRAME_REL_LIBS}
            DESTINATION "${CURRENT_PACKAGES_DIR}/lib")
    endif()
endif()

# Install debug libraries
if(NOT VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL "debug")
    file(GLOB WEBFRAME_DBG_LIBS
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/src/*.lib"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/src/*.a"
    )
    if(WEBFRAME_DBG_LIBS)
        file(INSTALL ${WEBFRAME_DBG_LIBS}
            DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib")
    endif()
endif()

# Generate cmake config files
set(WEBFRAME_CONFIG_PATH "${CURRENT_PACKAGES_DIR}/share/webframe")
file(MAKE_DIRECTORY "${WEBFRAME_CONFIG_PATH}")

file(WRITE "${WEBFRAME_CONFIG_PATH}/webframe-config.cmake"
[=[include_guard(GLOBAL)
include(CMakeFindDependencyMacro)

find_dependency(Libevent CONFIG REQUIRED)
find_dependency(wxWidgets CONFIG REQUIRED)

get_filename_component(_webframe_root "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

function(_webframe_add_imported_target _name _libname _iface_libs)
    if(NOT TARGET webframe::${_name})
        add_library(webframe::${_name} STATIC IMPORTED)
        find_library(_webframe_${_name}_rel
            NAMES ${_libname}
            PATHS "${_webframe_root}/lib"
            NO_DEFAULT_PATH)
        find_library(_webframe_${_name}_dbg
            NAMES ${_libname}
            PATHS "${_webframe_root}/debug/lib"
            NO_DEFAULT_PATH)
        if(_webframe_${_name}_rel)
            set_property(TARGET webframe::${_name} APPEND PROPERTY
                IMPORTED_CONFIGURATIONS RELEASE)
            set_target_properties(webframe::${_name} PROPERTIES
                IMPORTED_LOCATION_RELEASE "${_webframe_${_name}_rel}")
        endif()
        if(_webframe_${_name}_dbg)
            set_property(TARGET webframe::${_name} APPEND PROPERTY
                IMPORTED_CONFIGURATIONS DEBUG)
            set_target_properties(webframe::${_name} PROPERTIES
                IMPORTED_LOCATION_DEBUG "${_webframe_${_name}_dbg}")
        endif()
        set_target_properties(webframe::${_name} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${_webframe_root}/include"
            INTERFACE_LINK_LIBRARIES "${_iface_libs}")
    endif()
endfunction()

_webframe_add_imported_target(server webframe_server "libevent::core;libevent::extra")
if(WIN32)
    _webframe_add_imported_target(desktop webframe_desktop "wx::core;wx::webview")
    set_property(TARGET webframe::desktop APPEND PROPERTY
        INTERFACE_COMPILE_DEFINITIONS WEBFRAME_DESKTOP_RUNTIME)
endif()
]=])

file(WRITE "${WEBFRAME_CONFIG_PATH}/webframe-config-version.cmake"
[=[set(PACKAGE_VERSION "0.1.0")
if(PACKAGE_FIND_VERSION VERSION_GREATER PACKAGE_VERSION)
    set(PACKAGE_VERSION_COMPATIBLE FALSE)
else()
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
    if(PACKAGE_FIND_VERSION STREQUAL PACKAGE_VERSION)
        set(PACKAGE_VERSION_EXACT TRUE)
    endif()
endif()
]=])

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
