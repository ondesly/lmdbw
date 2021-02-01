//
//  dbw.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 30.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include "lmdbw/column.h"
#include "lmdbw/db.h"
#include "lmdbw/serializer.h"
#include "lmdbw/transaction.h"

namespace lm {

    template<class K, class V>
    class dbw {
    public:

        dbw(const lm::db::env_params &env_params, const lm::db::dbi_params &dbi_params,
            const std::vector<lm::column<V>> &columns)
                : m_columns(columns) {
            m_db = std::make_unique<lm::db>(env_params, dbi_params);
        }

    public:

        void put(const K &key, const V &value) {
            serializer<V> ser{m_columns};

            const val k = {reinterpret_cast<const uint8_t *>(&key), sizeof(K)};
            const val v = ser.encode(value);

            transaction transaction{*m_db};
            transaction.put(k, v);
        }

        V get(K key) {
            transaction transaction(*m_db);

            const val k = {reinterpret_cast<const uint8_t *>(&key), sizeof(K)};
            const val v = transaction.get(k);

            serializer<V> ser{m_columns};
            return ser.decode(v);
        }

        void del(K key) {
            const val k = {reinterpret_cast<const uint8_t *>(&key), sizeof(K)};

            transaction transaction(*m_db);
            transaction.del(k);
        }

    private:

        std::unique_ptr<lm::db> m_db;
        std::vector<lm::column<V>> m_columns;

    };

}