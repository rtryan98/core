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
