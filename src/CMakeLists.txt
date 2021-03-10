#
#  CMakeLists.txt
#  lmdbw
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
#  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
#

find_package(LMDB REQUIRED)

#

add_library(lmdbw STATIC
        lmdbw/cursor.cpp
        lmdbw/db.cpp
        lmdbw/transaction.cpp)

target_link_libraries(lmdbw PRIVATE lmdb)

#

set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)

set(PUBLIC_HEADERS
        ${INCLUDE_DIR}/lmdbw/column.h
        ${INCLUDE_DIR}/lmdbw/cursor.h
        ${INCLUDE_DIR}/lmdbw/db.h
        ${INCLUDE_DIR}/lmdbw/dbw.h
        ${INCLUDE_DIR}/lmdbw/exception.h
        ${INCLUDE_DIR}/lmdbw/pack.h
        ${INCLUDE_DIR}/lmdbw/serializer.h
        ${INCLUDE_DIR}/lmdbw/transaction.h
        ${INCLUDE_DIR}/lmdbw/val.h)

set_target_properties(lmdbw PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_include_directories(lmdbw PUBLIC $<BUILD_INTERFACE:${INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)

#

set_target_properties(lmdbw PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)