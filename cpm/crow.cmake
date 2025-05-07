include(CPM)
include(asio)
CPMAddPackage(
    NAME crow
    VERSION 1.2.1.2
    GITHUB_REPOSITORY CrowCpp/Crow
    OPTIONS
        "CROW_BUILD_EXAMPLES OFF"
        "CROW_BUILD_TOOLS OFF"
        "CROW_BUILD_TESTS OFF"
        "CROW_BUILD_DOCS OFF"
)
