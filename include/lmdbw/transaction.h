//
//  transaction.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <cstddef>
#include <cstdint>

#include "lmdbw/val.h"

struct MDB_txn;

namespace lm {

    namespace flag {

        namespace transaction {

            const int rd_only = 0x20000; // Read only.

        }

        namespace put {

            const int no_dup_data = 0x20; // Only for dup_sort. Don't write if the key and data pair already exist.
            const int no_overwrite = 0x10; // Don't write if the key already exists.
            const int reserve = 0x10000; // Just reserve space for data, don't copy it. Return a pointer to the reserved space.
            const int append = 0x20000; // Data is being appended, don't split full pages.
            const int append_dup = 0x40000; // Duplicate data is being appended, don't split full pages.

        }

    }

    class db;

    class transaction {
    public:

        explicit transaction(const db &db, uint32_t flags = 0);

        ~transaction();

    public:

        MDB_dbi get_dbi() const;

        MDB_txn *get_transaction() const;

        size_t get_count();

        void put(const val &key, const val &value, uint32_t flags = 0);

        val get(const val &key);

        void del(const val &key);

    private:

        const db &m_db;
        MDB_txn *m_txn;
        uint32_t m_flags;

    };

}