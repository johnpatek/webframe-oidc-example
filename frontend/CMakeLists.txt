find_program(NPM_EXECUTABLE npm REQUIRED)

# Target to run 'npm install' - does not run by default
add_custom_target(npm_install
    COMMAND ${NPM_EXECUTABLE} install
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running npm install in frontend"
)

# Discover and create 'npm run build' targets for each app
file(GLOB app_dirs LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/apps/*")

foreach(app_dir ${app_dirs})
    if(IS_DIRECTORY "${app_dir}")
        get_filename_component(app_name "${app_dir}" NAME)

        add_custom_target(build_${app_name}
            COMMAND ${NPM_EXECUTABLE} run build -w apps/${app_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS npm_install
            COMMENT "Building ${app_name} app"
            VERBATIM
        )
    endif()
endforeach()
