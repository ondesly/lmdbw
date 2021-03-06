//
//  cursor.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 03.02.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <algorithm>
#include <cerrno>

#include <liblmdb/lmdb.h>

#include "lmdbw/cursor.h"
#include "lmdbw/exception.h"
#include "lmdbw/transaction.h"

lm::cursor::cursor(const lm::transaction &transaction, const lm::val &begin, const lm::val &end)
        : m_transaction(transaction) {
    if (const auto rc = mdb_cursor_open(transaction.get_transaction(), transaction.get_dbi(), &m_cursor)) {
        throw lm::exception{"cursor::mdb_cursor_open", rc};
    }

    //

    set_end(end);
    set_begin(begin);
}

void lm::cursor::set_begin(const lm::val &begin) {
    if (begin.size == 0) {
        set(MDB_FIRST);
    } else {
        set(MDB_SET_RANGE, begin);
    }
}

void lm::cursor::set_end(const lm::val &end) {
    if (end.size == 0) {
        m_end_key = {};
    } else {
        set(MDB_SET_RANGE, end);

        auto current_key = to<MDB_val>(m_current.first);
        auto end_key = to<MDB_val>(end);

        if (mdb_cmp(m_transaction.get_transaction(), m_transaction.get_dbi(), &current_key, &end_key) == 0) {
            set(MDB_NEXT);
        }

        m_end_key = val::copy(m_current.first);
    }
}

lm::cursor::~cursor() {
    delete[] m_end_key.data;

    mdb_cursor_close(m_cursor);
}

lm::cursor::iterator lm::cursor::begin() {
    return {*this, m_current.first};
}

lm::cursor::iterator lm::cursor::end() {
    return {*this, m_end_key};
}

void lm::cursor::set(int option, const lm::val &key) {
    auto mdb_key = to<MDB_val>(key);
    MDB_val mdb_data;

    if (const int rc = mdb_cursor_get(m_cursor, &mdb_key, &mdb_data, MDB_cursor_op(option))) {
        if (rc != MDB_NOTFOUND && rc != EINVAL) {
            throw lm::exception{"cursor::mdb_cursor_get", rc};
        } else {
            m_current = {};
        }
    } else {
        m_current = {to_val(mdb_key), to_val(mdb_data)};
    }
}

void lm::cursor::del() {
    if (const int rc = mdb_cursor_del(m_cursor, 0)) {
        throw lm::exception{"cursor::mdb_cursor_del", rc};
    }
}

// -- iterator --

lm::cursor::iterator::iterator(lm::cursor &cursor, const lm::val &key) : m_cursor(cursor), m_key(key) {}

lm::cursor::iterator::reference lm::cursor::iterator::operator*() const {
    return m_cursor.m_current;
}

lm::cursor::iterator &lm::cursor::iterator::operator++() {
    m_cursor.set(MDB_NEXT);

    return *this;
}

void lm::cursor::iterator::del() {
    m_cursor.del();
    m_cursor.set(MDB_GET_CURRENT);
}

bool lm::cursor::iterator::operator==(const lm::cursor::iterator &other) const {
    return std::equal(m_key.data, m_key.data + m_key.size, other.m_key.data, other.m_key.data + other.m_key.size);
}

bool lm::cursor::iterator::operator!=(const lm::cursor::iterator &other) const {
    return !(operator==(other));
}