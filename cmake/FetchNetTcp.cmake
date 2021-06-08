include(FetchContent)

set(NETTCP_REPOSITORY "https://github.com/OlivierLDff/NetTcp.git" CACHE STRING "NetTcp repository url")
set(NETTCP_TAG master CACHE STRING "NetTcp git tag")

FetchContent_Declare(
  NetTcp
  GIT_REPOSITORY ${NETTCP_REPOSITORY}
  GIT_TAG        ${NETTCP_TAG}
)

set(NETTCP_ENABLE_QML OFF CACHE BOOL "Embedded Debug Qml module for NetTcp class")
set(NETTCP_FOLDER_PREFIX "Dependencies" CACHE STRING "Prefix folder for all NetTcp generated targets in generated project (only decorative)")
FetchContent_MakeAvailable(NetTcp)
