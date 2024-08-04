aux_source_directory(${CMAKE_CURRENT_LIST_DIR} WA_SRC_LIST)

add_executable(
    ${PROJECT_NAME}
    ${WA_SRC_LIST}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    wa-thirdparty
)