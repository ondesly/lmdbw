//
//  transaction.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <liblmdb/lmdb.h>

#include "lmdbw/db.h"
#include "lmdbw/exception.h"
#include "lmdbw/transaction.h"

lm::transaction::transaction(const lm::db &db, uint32_t flags) : m_db(db), m_flags(flags) {
    if (const auto rc = mdb_txn_begin(m_db.get_env(), nullptr, m_flags, &m_txn)) {
        throw lm::exception{"transaction::mdb_txn_begin", rc};
    }
}

lm::transaction::~transaction() {
    if (m_txn) {
        if (m_flags & flag::transaction::rd_only) {
            mdb_txn_abort(m_txn);
        } else {
            mdb_txn_commit(m_txn);
        }
    }
}

MDB_dbi lm::transaction::get_dbi() const {
    return m_db.get_dbi();
}

MDB_txn *lm::transaction::get_transaction() const {
    return m_txn;
}

void lm::transaction::put(const lm::val &key, const lm::val &value, uint32_t flags) {
    MDB_val mdb_key{key.size, const_cast<void *>(static_cast<const void *>(key.data))};
    MDB_val mdb_value{value.size, const_cast<void *>(static_cast<const void *>(value.data))};

    if (const auto rc = mdb_put(m_txn, m_db.get_dbi(), &mdb_key, &mdb_value, flags)) {
        mdb_txn_abort(m_txn);
        m_txn = nullptr;
        throw lm::exception{"transaction::mdb_put", rc};
    }
}

lm::val lm::transaction::get(const lm::val &key) {
    MDB_val mdb_key{key.size, const_cast<void *>(static_cast<const void *>(key.data))};
    MDB_val mdb_value{};

    if (const auto rc = mdb_get(m_txn, m_db.get_dbi(), &mdb_key, &mdb_value)) {
        if (rc != MDB_NOTFOUND) {
            throw lm::exception{"transaction::mdb_get", rc};
        } else {
            return {};
        }
    }

    return {static_cast<uint8_t *>(mdb_value.mv_data), mdb_value.mv_size};
}

void lm::transaction::del(const lm::val &key) {
    MDB_val mdb_key{key.size, const_cast<void *>(static_cast<const void *>(key.data))};

    if (const auto rc = mdb_del(m_txn, m_db.get_dbi(), &mdb_key, nullptr)) {
        if (rc != MDB_NOTFOUND) {
            throw lm::exception{"transaction::mdb_del", rc};
        }
    }
}