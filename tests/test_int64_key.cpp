//
//  test_int64_key.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.07.2024.
//  Copyright © 2024 Dmitrii Torkhov. All rights reserved.
//

#include <cassert>
#include <vector>

#include <lmdbw/cursor.h>
#include <lmdbw/db.h>
#include <lmdbw/transaction.h>

int main() {
    { // No int64 key flag
        const lm::db db({"./db.db", lm::flag::env::no_subdir}, {"int64_key_default", lm::flag::dbi::create});
        lm::transaction transaction(db);

        constexpr uint64_t key_value_0 = 0x0000FF;
        constexpr uint64_t key_value_1 = 0x00FF00;
        constexpr uint64_t key_value_2 = 0xFF0000;

        transaction.put({reinterpret_cast<const uint8_t *>(&key_value_2), sizeof(key_value_2)}, {});
        transaction.put({reinterpret_cast<const uint8_t *>(&key_value_0), sizeof(key_value_0)}, {});
        transaction.put({reinterpret_cast<const uint8_t *>(&key_value_1), sizeof(key_value_1)}, {});
        assert(transaction.get_count() == 3);

        std::vector<uint64_t> keys;
        for (const auto &[key, value] : lm::cursor{transaction}) {
            const auto key_64 = *reinterpret_cast<const uint64_t *>(key.data);
            keys.push_back(key_64);
        }
        assert(keys.size() == 3);
        assert(keys[0] == key_value_2);
        assert(keys[1] == key_value_1);
        assert(keys[2] == key_value_0);
    }

    { // No int64 key flag
        const lm::db db({"./db.db", lm::flag::env::no_subdir},
                        {"int64_key_flagged", lm::flag::dbi::create, lm::flag::custom::integer_64_key});
        lm::transaction transaction(db);

        constexpr uint64_t key_value_0 = 0x0000FF;
        constexpr uint64_t key_value_1 = 0x00FF00;
        constexpr uint64_t key_value_2 = 0xFF0000;

        transaction.put({reinterpret_cast<const uint8_t *>(&key_value_2), sizeof(key_value_2)}, {});
        transaction.put({reinterpret_cast<const uint8_t *>(&key_value_0), sizeof(key_value_0)}, {});
        transaction.put({reinterpret_cast<const uint8_t *>(&key_value_1), sizeof(key_value_1)}, {});
        assert(transaction.get_count() == 3);

        std::vector<uint64_t> keys;
        for (const auto &[key, value] : lm::cursor{transaction}) {
            const auto key_64 = *reinterpret_cast<const uint64_t *>(key.data);
            keys.push_back(key_64);
        }
        assert(keys.size() == 3);
        assert(keys[0] == key_value_0);
        assert(keys[1] == key_value_1);
        assert(keys[2] == key_value_2);
    }

    return 0;
}
