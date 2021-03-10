#
#  CMakeLists.txt
#  lmdbw
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
#  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
#

find_package(LMDB REQUIRED)

add_library(lmdbw STATIC
        lmdbw/cursor.cpp
        lmdbw/db.cpp
        lmdbw/transaction.cpp)

set(PUBLIC_HEADERS
        include/lmdbw/column.h
        include/lmdbw/cursor.h
        include/lmdbw/db.h
        include/lmdbw/dbw.h
        include/lmdbw/exception.h
        include/lmdbw/pack.h
        include/lmdbw/serializer.h
        include/lmdbw/transaction.h
        include/lmdbw/val.h)

set_target_properties(lmdbw PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_link_libraries(lmdbw PRIVATE lmdb)

target_include_directories(lmdbw PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
        $<INSTALL_INTERFACE:include>)

set_target_properties(lmdbw PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)