cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# declare project
project(mantis VERSION 0.1.0)

# generate version config an supported drivers list
configure_file(cmake_templates/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/mantis/version.h)

# CXX standard
set(CMAKE_CXX_STANDARD 20)

#####################################################################################
##                                 Compiler Flags                                  ##
#####################################################################################

# add warnings as errors and all warnings
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Werror>
        $<$<COMPILE_LANGUAGE:CXX>:-Wall>)

# compile as hidden for linking
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fvisibility=hidden>)

# compile with position independent code
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fPIC>)

# Set build optimisation options for the native arch
add_compile_options(
        $<$<COMPILE_LANGUAGE:CXX>:-march=native>
        $<$<COMPILE_LANGUAGE:CXX>:-mtune=native>)

# Set the default build type to release
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified: defaulting to release.")
endif()

#####################################################################################
##                                 Dependencies                                    ##
#####################################################################################
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

if(PYTHON OR DEFINED SKBUILD) # python on request or pip install
  find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
  set(PYBIND11_FINDPYTHON ON)
  find_package(pybind11 CONFIG REQUIRED)
  message(STATUS "pybind11 found, enabling PyMantis")
  set(PyMantisEnabled TRUE)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-DM_PYMANTIS=TRUE>) # ifdef pymantis flag
endif()

# supported driver list for perror messaging
set(MANTIS_SUPPORTED_DRIVERS "UHD")

# get all mantis files EXCEPT Driver specific ones
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/*.cpp)
list(APPEND SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp)

#####################################################################################
##                                  relevant drivers                               ##
#####################################################################################

# uhd
find_package(UHD)
if(UHD_FOUND)
  message("Compiling with UHD support")
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-DM_UHD=TRUE>) # ifdef uhd flag
  list(APPEND MANTIS_DRIVER_LIBS uhd) # update libs var

  file(GLOB_RECURSE MANTIS_UHD_SOURCE_FILES src/driver_impl/uhd/*.cpp) # add include uhd source files
  list(APPEND SOURCE_FILES ${MANTIS_UHD_SOURCE_FILES})
  if(NOT DEFINED SKBUILD)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/mantis/driver_impl/uhd DESTINATION include/mantis/driver_impl/) # install mantis uhd impl includes
  endif()
else()
  message("Compiling without UHD support")
endif()

# add the "virtual" driver
file(GLOB_RECURSE MANTIS_VIRTUAL_SOURCE_FILES src/driver_impl/virtual/*.cpp) # add include virtual source files
list(APPEND SOURCE_FILES ${MANTIS_VIRTUAL_SOURCE_FILES})

# install virtual msdr includes
if(NOT DEFINED SKBUILD)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/mantis/driver_impl/virtual DESTINATION include/mantis/driver_impl/) # install mantis virtual impl includes
endif()

# make sure we found at least one driver library
if("${MANTIS_DRIVER_LIBS}" STREQUAL "")
  message(WARNING "[Warning] No supported drivers found.\nSupported drivers are: ${MANTIS_SUPPORTED_DRIVERS}\nCompiling with virtual functionality only")
endif()

# generate build_info file
configure_file(cmake_templates/build_info.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/mantis/build_info.h)

#####################################################################################
##                                   main library                                  ##
#####################################################################################

# main library
if(DEFINED SKBUILD) # need static linking for pip install
  add_library(mantis STATIC ${SOURCE_FILES})
else()
  add_library(mantis SHARED ${SOURCE_FILES})
endif()

# link libs
target_link_libraries(mantis PUBLIC Threads::Threads nlohmann_json::nlohmann_json Boost::program_options ${MANTIS_DRIVER_LIBS})

# include dirs
target_include_directories(mantis PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# version
set_target_properties(mantis PROPERTIES VERSION ${PROJECT_VERSION})

#####################################################################################
##                                   PyMantis                                      ##
#####################################################################################

if(PyMantisEnabled)
  pybind11_add_module(mantis_python python/pymantis.cpp)
  target_link_libraries(mantis_python PRIVATE mantis)
  set_target_properties(mantis_python PROPERTIES OUTPUT_NAME mantis)
  if(DEFINED SKBUILD)
    set_target_properties(mantis_python PROPERTIES INSTALL_RPATH "$ORIGIN")
    install(TARGETS mantis_python DESTINATION mantis)
    install(DIRECTORY python/stubs/mantis/ DESTINATION mantis FILES_MATCHING PATTERN "*.pyi")
    install(FILES python/__init__.py python/py.typed DESTINATION mantis)
    return() # no need to continue cpp install on pip build
  else()
    install(TARGETS mantis_python DESTINATION "${Python3_SITEARCH}/mantis")
    install(DIRECTORY python/stubs/mantis/ DESTINATION "${Python3_SITEARCH}/mantis" FILES_MATCHING PATTERN "*.pyi")
    install(FILES python/__init__.py python/py.typed DESTINATION "${Python3_SITEARCH}/mantis")
  endif()
endif()

#####################################################################################
##                               library installation                              ##
#####################################################################################

# additional mantis files
file(GLOB OTHER_MANTIS_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/mantis/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/mantis/*.hpp)

include(GNUInstallDirs)

#install include dir
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/mantis/lib DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mantis)

install(FILES ${OTHER_MANTIS_INCLUDES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mantis)

install(TARGETS mantis
        EXPORT MantisTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT MantisTargets
        FILE MantisTargets.cmake
        NAMESPACE mantis::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mantis
)

#####################################################################################
##                                   executables                                   ##
#####################################################################################

# mantis
add_executable(mantis_bin ${CMAKE_CURRENT_SOURCE_DIR}/src/mantis.cpp ${MANTIS_BINARIES_ADDITIONAL_SOURCES})
target_link_libraries(mantis_bin PRIVATE mantis)
set_target_properties(mantis_bin PROPERTIES OUTPUT_NAME mantis)
list(APPEND MANTIS_BINARY_TARGETS mantis_bin)

install(TARGETS ${MANTIS_BINARY_TARGETS} DESTINATION bin)

#####################################################################################
##                MantisConfig.cmake and MantisConfigVersion.cmake                 ##
#####################################################################################

# mantis config
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/MantisConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/MantisConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mantis)

# version config
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/cmake/MantisConfigVersion.cmake
        VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion)


#####################################################################################
##                               .cmake installations                              ##
#####################################################################################
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/MantisConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mantis)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/MantisConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mantis)

# install config json
install(FILES ${PROJECT_SOURCE_DIR}/mantis-config.json DESTINATION $ENV{HOME}/.config/mantis)

install(CODE "message(\"Mantis v${PROJECT_VERSION} successfully installed.\nPlease run 'sudo ldconfig' to update the library cache.\")")
