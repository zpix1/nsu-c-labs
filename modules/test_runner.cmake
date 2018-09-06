set(TEST_RUNNER_NAME "test${PROJECT_NAME}.exe")

include (CTest)
enable_testing()

add_test(
        NAME ${TEST_RUNNER_NAME}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/${TEST_RUNNER_NAME} ${PROJECT_NAME}
)

set_tests_properties(${TEST_RUNNER_NAME} PROPERTIES DEPENDS ${PROJECT_NAME})
