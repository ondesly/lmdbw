//
//  test_cursor.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.07.2024.
//  Copyright © 2024 Dmitrii Torkhov. All rights reserved.
//

#include <cassert>

#include <lmdbw/cursor.h>
#include <lmdbw/db.h>
#include <lmdbw/transaction.h>

int main() {
    const lm::db db({"./db.db", lm::flag::env::no_subdir}, {"cursor", lm::flag::dbi::create});

    { // One value
        lm::transaction transaction(db);

        constexpr uint64_t key_value = 16;
        transaction.put({reinterpret_cast<const uint8_t *>(&key_value), sizeof(key_value)}, {});
        assert(transaction.get_count() == 1);

        constexpr uint64_t begin_key_value = 8;
        const lm::val begin_key{reinterpret_cast<const uint8_t *>(&begin_key_value), sizeof(begin_key_value)};
        constexpr uint64_t end_key_value = 32;
        const lm::val end_key{reinterpret_cast<const uint8_t *>(&end_key_value), sizeof(end_key_value)};

        size_t counter = 0;
        for (const auto &[key, value] : lm::cursor{transaction, begin_key, end_key}) {
            ++counter;
        }
        assert(counter == 1);
    }

    return 0;
}
