#
#  FindLMDB.cmake
#  lmdbw
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
#  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
#

add_library(lmdb STATIC
        ${lmdb_SOURCE_DIR}/libraries/liblmdb/mdb.c
        ${lmdb_SOURCE_DIR}/libraries/liblmdb/midl.c)

target_include_directories(lmdb PUBLIC
        $<BUILD_INTERFACE:${lmdb_SOURCE_DIR}/libraries>
        $<INSTALL_INTERFACE:include>)