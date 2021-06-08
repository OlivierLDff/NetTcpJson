
include(${PROJECT_SOURCE_DIR}/cmake/FetchGTest.cmake)
find_package(Qt5 REQUIRED COMPONENTS Test)

set(NETTCPJSON_TESTS_TARGET "${NETTCPJSON_TARGET}Tests")

set(NETTCPJSON_TESTS_SRCS
  Tests.cpp
  ExtractorTests.cpp
  ServerTests.cpp
)

message(STATUS "Add Test: ${NETTCPJSON_TESTS_TARGET}")

add_executable(${NETTCPJSON_TESTS_TARGET} ${NETTCPJSON_TESTS_SRCS})
target_link_libraries(${NETTCPJSON_TESTS_TARGET} PRIVATE
  ${NETTCPJSON_TARGET}
  gtest
  Qt::Test
)
set_target_properties(${NETTCPJSON_TESTS_TARGET} PROPERTIES FOLDER "Tests")

add_test(NAME ${NETTCPJSON_TESTS_TARGET} COMMAND ${NETTCPJSON_TESTS_TARGET})
