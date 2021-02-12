//
//  db.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <mutex>
#include <string>
#include <sys/stat.h>
#include <unordered_map>

struct MDB_env;
typedef unsigned int MDB_dbi;
typedef mode_t mdb_mode_t;

namespace lm {

    namespace flag {

        namespace env {

            const int fixed_map = 0x01; // Mmap at a fixed address (experimental).
            const int no_subdir = 0x4000; // No environment directory.
            const int no_sync = 0x10000; // Don't fsync after commit.
            const int rdonly = 0x20000; // Read only.
            const int no_meta_sync = 0x40000; // Don't fsync metapage after commit.
            const int write_map = 0x80000; // Use writable mmap.
            const int map_async = 0x100000; // Use asynchronous msync when write_map is used.
            const int no_tls = 0x200000; // Tie reader locktable slots to transaction objects instead of to threads.
            const int no_lock = 0x400000; // Don't do any locking, caller must manage their own locks.
            const int no_rd_ahead = 0x800000; // Don't do readahead (no effect on windows).
            const int no_mem_init = 0x1000000; // Don't initialize malloc'd memory before writing to datafile.

        }

        namespace dbi {

            const int reverse_key = 0x02; // Use reverse string keys.
            const int dup_sort = 0x04; // Use sorted duplicates.
            const int integer_key = 0x08; // Numeric keys in native byte order: either unsigned int or size_t. the keys must all be of the same size.
            const int dup_fixed = 0x10; // With dup_sort, sorted dup items have fixed size.
            const int integer_dup = 0x20; // With dup_sort, dups are integer_key-style integers.
            const int reverse_dup = 0x40; // With dup_sort, use reverse string dups.
            const int create = 0x40000; // Create db if not already existing.

        }

    }

    class db {
    public:

        struct env_params {
            std::string path;
            uint32_t flags;
            size_t map_size;
            MDB_dbi max_dbs;
            mdb_mode_t open_mode;
        };

        struct dbi_params {
            std::string name;
            uint32_t flags;
        };

    public:

        db(const env_params &env_params, const dbi_params &dbi_params);

    public:

        MDB_env *get_env() const;

        MDB_dbi get_dbi() const;

    private:

        static std::mutex s_mutex;

        static std::unordered_map<std::string, MDB_env *> s_envs;
        static std::unordered_map<std::string, MDB_dbi> s_dbis;

    private:

        static MDB_env *open_env(const env_params &env_params);

        static MDB_dbi open_dbi(MDB_env *env, const dbi_params &dbi_params);

    private:

        MDB_env *m_env = nullptr;
        MDB_dbi m_dbi = 0;

    };

}