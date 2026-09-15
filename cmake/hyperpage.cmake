find_package(hyperpage CONFIG REQUIRED)

# create_hyperpage_archive(
#   TARGET <target_name>
#   DIST_DIR <path_to_dist_folder>
#   OUTPUT_FILE <output_file_path>
#   [DEPENDS ...])
#
# Creates a custom target that packs a directory into a hyperpage archive.
function(create_hyperpage_archive)
    set(options)
    set(oneValueArgs TARGET DIST_DIR OUTPUT_FILE)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(ARG
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(NOT ARG_TARGET)
        message(FATAL_ERROR "TARGET is required for create_hyperpage_archive")
    endif()

    if(NOT ARG_DIST_DIR)
        message(FATAL_ERROR "DIST_DIR is required for create_hyperpage_archive")
    endif()

    if(NOT ARG_OUTPUT_FILE)
        message(FATAL_ERROR "OUTPUT_FILE is required for create_hyperpage_archive")
    endif()

    add_custom_command(
        OUTPUT "${ARG_OUTPUT_FILE}"
        COMMAND hyperpage::hyperpack --output "${ARG_OUTPUT_FILE}" "${ARG_DIST_DIR}"
        DEPENDS ${ARG_DEPENDS}
        COMMENT "Packing ${ARG_DIST_DIR} with hyperpack"
        VERBATIM
    )

    add_custom_target(${ARG_TARGET} ALL
        DEPENDS "${ARG_OUTPUT_FILE}"
    )
endfunction()