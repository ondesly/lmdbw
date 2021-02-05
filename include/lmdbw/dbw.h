//
//  dbw.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 30.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <unordered_map>

#include "lmdbw/column.h"
#include "lmdbw/cursor.h"
#include "lmdbw/db.h"
#include "lmdbw/serializer.h"
#include "lmdbw/transaction.h"

namespace lm {

    namespace {

        template<class K>
        lm::val encode_key(const K &key) {
            return {reinterpret_cast<const uint8_t *>(&key), sizeof(K)};
        }

        template<class K>
        K decode_key(const lm::val &key) {
            return *reinterpret_cast<const K *>(key.data);
        }

        lm::val encode_key(const std::string &key) {
            return {reinterpret_cast<const uint8_t *>(key.data()), key.size()};
        }

        std::string decode_key(const lm::val &key) {
            return {key.data, key.data + key.size};
        }

    }

    template<class K, class V>
    class dbw {
    public:

        dbw(const lm::db::env_params &env_params, const lm::db::dbi_params &dbi_params,
            const std::vector<lm::column<V>> &columns = {})
                : m_columns(columns) {
            m_db = std::make_unique<lm::db>(env_params, dbi_params);
        }

    public:

        size_t get_count() {
            transaction transaction{*m_db, flag::transaction::rd_only};
            return transaction.get_count();
        }

        void put(const K &key, const V &value) {
            serializer<V> ser{m_columns};

            const auto k = encode_key(key);
            const auto v = ser.encode(value);

            transaction transaction{*m_db};
            transaction.put(k, v);
        }

        void put(const std::unordered_map<K, V> &values) {
            transaction transaction{*m_db};
            serializer<V> ser{m_columns};

            for (const auto &[key, value] : values) {
                const auto k = encode_key(key);
                const auto v = ser.encode(value);

                transaction.put(k, v);
            }
        }

        V get(const K &key) {
            transaction transaction{*m_db, flag::transaction::rd_only};

            const auto k = encode_key(key);
            const auto v = transaction.get(k);

            serializer<V> ser{m_columns};
            return ser.decode(v);
        }

        std::unordered_map<K, V> get(const K &begin, const K &end) {
            transaction transaction{*m_db, flag::transaction::rd_only};
            cursor cursor{transaction, encode_key(begin), encode_key(end)};
            serializer<V> ser{m_columns};

            std::unordered_map<K, V> values;
            for (const auto &[key, value] : cursor) {
                const auto k = decode_key<K>(key);
                const auto v = ser.decode(value);

                values.emplace(k, v);
            }

            return values;
        }

        void del(const K &key) {
            const auto k = encode_key(key);

            transaction transaction{*m_db};
            transaction.del(k);
        }

        void del(const K &begin, const K &end) {
            transaction transaction{*m_db};
            cursor cursor{transaction, encode_key(begin), encode_key(end)};

            auto it = cursor.begin();
            const auto it_end = cursor.end();
            while (it != it_end) {
                it.del();
            }
        }

    private:

        std::unique_ptr<lm::db> m_db;
        std::vector<lm::column<V>> m_columns;

    };

}