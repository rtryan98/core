function(core_download_and_extract_zip URL DST_FOLDER FOLDER_NAME)
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip)
        message(STATUS "Downloading ${URL} and unpacking to ${DST_FOLDER}/${FOLDER_NAME}.")
        file(
            DOWNLOAD
            ${URL}
            ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip
        )
    else()
        message(STATUS "${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip already exists. No download required.")
    endif()
    if(NOT EXISTS ${DST_FOLDER}/${FOLDER_NAME})
        message(STATUS "Extracting ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip to ${DST_FOLDER}/${FOLDER_NAME}.")
        file(
            ARCHIVE_EXTRACT
            INPUT ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip
            DESTINATION ${DST_FOLDER}/${FOLDER_NAME}
        )
    else()
        message(STATUS "${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip is already extracted to ${DST_FOLDER}/${FOLDER_NAME}.")
    endif()
endfunction()

function(core_deploy_files SRC DST)
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        set(DEPLOY_FILES_DST ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/${DST})
        message(STATUS "Copying ${SRC} to ${DEPLOY_FILES_DST}.")
        file(COPY ${SRC} DESTINATION ${DEPLOY_FILES_DST})
    endforeach()
endfunction()

function(core_deploy_d3d12 DST)
    message(STATUS "Deploying D3D12 Agility SDK to <executable_location>/${DST}/.")
    core_deploy_files(${core_lib_SOURCE_DIR}/thirdparty/d3d12_agility_sdk/build/native/bin/x64/D3D12Core.dll ${DST})
    core_deploy_files(${core_lib_SOURCE_DIR}/thirdparty/d3d12_agility_sdk/build/native/bin/x64/d3d12SDKLayers.dll ${DST})
endfunction()

function(core_add_shader_include_path SHADER_INCLUDE_PATH)
    if(NOT DEFINED CORE_JSON_INCLUDE_PATHS)
        set(CORE_JSON_INCLUDE_PATHS "${SHADER_INCLUDE_PATH}" PARENT_SCOPE)
    else()
        set(CORE_JSON_INCLUDE_PATHS "${CORE_JSON_INCLUDE_PATHS}\;${SHADER_INCLUDE_PATH}" PARENT_SCOPE)
    endif()
endfunction()

function(core_generate_shader_include_path_json_string)
    set(CORE_JSON_INCLUDE_PATHS_LIST ${CORE_JSON_INCLUDE_PATHS}) # CMake scopes are *fun*
    foreach(SHADER_INCLUDE_PATH ${CORE_JSON_INCLUDE_PATHS_LIST})

    endforeach()
endfunction()

function(core_generate_shader_include_path_json_file DST_PATHS)
    core_generate_shader_include_path_json_string()
endfunction()
