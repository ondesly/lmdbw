//
//  test_pack.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 11.02.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <vector>

#include <lmdbw/cursor.h>
#include <lmdbw/db.h>
#include <lmdbw/pack.h>
#include <lmdbw/transaction.h>

namespace {

    const char *c_env_path = "./db.db";
    const char *c_db_name = "test_pack";

}

int main() {
    const uint64_t part_0 = 100;
    const uint32_t part_1 = 10;
    const uint8_t part_2 = 1;

    const std::vector<uint8_t> value = {'t', 'e', 's', 't', '_', 'v', 'a', 'l', 'u', 'e'};

    //

    lm::db db{{c_env_path, lm::flag::env::no_subdir},
              {c_db_name,  lm::flag::dbi::create | lm::flag::dbi::reverse_key}};

    //

    {
        lm::transaction write_txn{db};

        const lm::pack key{part_0, part_1, part_2};
        write_txn.put(key, {value.data(), value.size()});
    }

    //

    lm::transaction read_txn{db, lm::flag::transaction::rd_only};

    std::tuple<uint64_t, uint32_t, uint8_t> parts;
    std::vector<uint8_t> result_value;
    for (const auto &[key, value] : lm::cursor{read_txn}) {
        const lm::pack<uint64_t, uint32_t, uint8_t> keys{key.data};
        parts = keys.get();

        result_value.assign(value.data, value.data + value.size);
    }

    //

    return (part_0 == std::get<0>(parts) && part_1 == std::get<1>(parts) && part_2 == std::get<2>(parts) &&
            value == result_value) ? 0 : 1;
}