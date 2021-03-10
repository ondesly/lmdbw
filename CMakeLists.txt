#
#  CMakeLists.txt
#  lmdbw
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
#  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
#

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(lmdbw
        VERSION 1.0
        LANGUAGES C CXX)

set(CMAKE_XCODE_ATTRIBUTE_EXCLUDED_ARCHS[sdk=iphonesimulator*] "arm64")

################
# Dependencies #
################

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

#

include(FetchContent)

#

FetchContent_Declare(
        lmdb
        GIT_REPOSITORY https://github.com/LMDB/lmdb.git
        GIT_TAG LMDB_0.9.24
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)

#

FetchContent_MakeAvailable(lmdb)

#########
# Build #
#########

add_subdirectory(src)

###########
# Install #
###########

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} lmdb
        EXPORT ${PROJECT_NAME}-config
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}${INSTALL_SUFFIX}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}${INSTALL_SUFFIX}/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}-config DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

###########
# Testing #
###########

enable_testing()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif ()