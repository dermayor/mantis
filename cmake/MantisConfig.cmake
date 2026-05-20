include(CMakeFindDependencyMacro)

find_dependency(Threads REQUIRED)
find_dependency(nlohmann_json REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/MantisTargets.cmake)
