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
    if (const auto rc = mdb_txn_begin(m_db.get_env(), nullptr, m_flags, &m_transaction)) {
        throw lm::exception{"transaction::mdb_txn_begin", rc};
    }
}

lm::transaction::~transaction() {
    if (m_transaction) {
        if (m_flags & flag::transaction::rd_only) {
            mdb_txn_abort(m_transaction);
        } else {
            mdb_txn_commit(m_transaction);
        }
    }
}

MDB_dbi lm::transaction::get_dbi() const {
    return m_db.get_dbi();
}

MDB_txn *lm::transaction::get_transaction() const {
    return m_transaction;
}

size_t lm::transaction::get_count() {
    MDB_stat stat;
    if (const int rc = mdb_stat(m_transaction, m_db.get_dbi(), &stat)) {
        throw lm::exception{"transaction::mdb_stat", rc};
    }

    return stat.ms_entries;
}

void lm::transaction::put(const lm::val &key, const lm::val &value, uint32_t flags) {
    auto mdb_key = lm::to<MDB_val>(key);
    auto mdb_value = lm::to<MDB_val>(value);

    if (const auto rc = mdb_put(m_transaction, m_db.get_dbi(), &mdb_key, &mdb_value, flags)) {
        mdb_txn_abort(m_transaction);
        m_transaction = nullptr;
        throw lm::exception{"transaction::mdb_put", rc};
    }
}

lm::val lm::transaction::get(const lm::val &key) {
    auto mdb_key = lm::to<MDB_val>(key);
    MDB_val mdb_value;

    if (const auto rc = mdb_get(m_transaction, m_db.get_dbi(), &mdb_key, &mdb_value)) {
        if (rc != MDB_NOTFOUND) {
            throw lm::exception{"transaction::mdb_get", rc};
        } else {
            return {};
        }
    }

    return lm::to_val(mdb_value);
}

void lm::transaction::del(const lm::val &key) {
    auto mdb_key = lm::to<MDB_val>(key);

    if (const auto rc = mdb_del(m_transaction, m_db.get_dbi(), &mdb_key, nullptr)) {
        if (rc != MDB_NOTFOUND) {
            throw lm::exception{"transaction::mdb_del", rc};
        }
    }
}