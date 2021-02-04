//
//  cursor.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 03.02.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <liblmdb/lmdb.h>

#include "lmdbw/cursor.h"
#include "lmdbw/exception.h"
#include "lmdbw/transaction.h"

lm::cursor::cursor(const lm::transaction &transaction, const lm::val &begin, const lm::val &end)
        : m_transaction(transaction), m_begin_key(begin), m_end_key(end) {
    if (const auto rc = mdb_cursor_open(transaction.get_transaction(), transaction.get_dbi(), &m_cursor)) {
        throw lm::exception{"cursor::mdb_cursor_open", rc};
    }

    //

    set_cursor(MDB_SET_RANGE, end);
    set_cursor(MDB_NEXT);
    m_end_key = m_current.first;

    set_cursor(MDB_SET_RANGE, begin);
    m_begin_key = m_current.first;
}

lm::cursor::~cursor() {
    mdb_cursor_close(m_cursor);
}

lm::cursor::iterator lm::cursor::begin() {
    return {*this, m_begin_key};
}

lm::cursor::iterator lm::cursor::end() {
    return {*this, m_end_key};
}

void lm::cursor::set_cursor(int option, const lm::val &key) {
    MDB_val mdb_key{key.size, const_cast<void *>(static_cast<const void *>(key.data))};
    MDB_val mdb_data;

    if (const int rc = mdb_cursor_get(m_cursor, &mdb_key, &mdb_data, MDB_cursor_op(option))) {
        if (rc != MDB_NOTFOUND) {
            throw lm::exception{"cursor::mdb_cursor_get", rc};
        } else {
            m_current = {};
        }
    } else {
        m_current = {{static_cast<uint8_t *>(mdb_key.mv_data),  mdb_key.mv_size},
                     {static_cast<uint8_t *>(mdb_data.mv_data), mdb_data.mv_size}};
    }
}

// -- iterator --

lm::cursor::iterator::iterator(lm::cursor &cursor, const lm::val &key)
        : m_cursor(cursor), m_key(key) {}

lm::cursor::iterator::reference lm::cursor::iterator::operator*() const {
    return m_cursor.m_current;
}

lm::cursor::iterator &lm::cursor::iterator::operator++() {
    m_cursor.set_cursor(MDB_NEXT);
    m_key = m_cursor.m_current.first;

    return *this;
}

bool lm::cursor::iterator::operator==(const lm::cursor::iterator &other) const {
    return m_key.data == other.m_key.data;
}

bool lm::cursor::iterator::operator!=(const lm::cursor::iterator &other) const {
    return !(operator==(other));
}