include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

set(NETTCP_REPOSITORY
    "https://github.com/OlivierLDff/NetTcp.git"
    CACHE STRING "NetTcp repository url"
)
set(NETTCP_TAG
    master
    CACHE STRING "NetTcp git tag"
)

set(NETTCP_FOLDER_PREFIX
    "Dependencies"
    CACHE STRING
          "Prefix folder for all NetTcp generated targets in generated project (only decorative)"
)

CPMAddPackage(
  NAME NetTcp
  GIT_REPOSITORY ${NETTCP_REPOSITORY}
  GIT_TAG ${NETTCP_TAG}
)
