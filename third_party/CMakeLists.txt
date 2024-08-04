aux_source_directory(${CMAKE_CURRENT_LIST_DIR} WA_THIRD_PARTY_SRC_LIST)

add_library(
    wa-thirdparty
    ${WA_THIRD_PARTY_SRC_LIST}
)

target_include_directories(wa-thirdparty PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/argparse
)
