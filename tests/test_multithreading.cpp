//
//  test_multithreading.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 05.02.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <map>
#include <memory>
#include <random>
#include <thread>
#include <vector>

#include <lmdbw/dbw.h>

namespace {

    const size_t c_threads_count = 8;
    const size_t c_put_count = 100;

    const char *c_env_path = "./db.db";
    const char *c_db_name = "multithreading";

}

int main() {

    // Clean up

    {
        lm::db db{{c_env_path, lm::flag::env::no_subdir},
                  {c_db_name,  lm::flag::dbi::create}};
        lm::transaction txn{db};
        lm::cursor cursor{txn};

        auto it = cursor.begin();
        const auto it_end = cursor.end();
        while (it != it_end) {
            it.del();
        }
    }

    // Fn

    using ref_map = std::map<std::vector<uint8_t>, std::vector<uint8_t>>;

    std::vector<ref_map> maps(c_threads_count);

    const auto fn = [](ref_map &map) {
        std::default_random_engine gen{std::random_device{}()};
        std::uniform_int_distribution<uint8_t> distrib{20, 255};

        lm::db db{{c_env_path, lm::flag::env::no_subdir},
                  {c_db_name,  lm::flag::dbi::create}};

        for (size_t i = 0; i < c_put_count; ++i) {
            std::vector<uint8_t> key(distrib(gen));
            std::generate(key.begin(), key.end(), gen);

            std::vector<uint8_t> value(distrib(gen));
            std::generate(value.begin(), value.end(), gen);

            map.emplace(key, value);

            lm::transaction txn(db);
            txn.put({key.data(), key.size()}, {value.data(), value.size()});
        }
    };

    // Run

    std::vector<std::shared_ptr<std::thread>> threads;
    for (size_t i = 0; i < c_threads_count; ++i) {
        threads.push_back(std::make_shared<std::thread>(fn, std::ref(maps[i])));
    }

    for (const auto &thread : threads) {
        thread->join();
    }

    // Unite

    ref_map super_map;
    for (const auto &map : maps) {
        super_map.insert(map.begin(), map.end());
    }

    // Test

    lm::db db{{c_env_path, lm::flag::env::no_subdir},
              {c_db_name,  lm::flag::dbi::create}};
    lm::transaction txn(db, lm::flag::transaction::rd_only);

    auto it = super_map.begin();
    for (const auto &[key, value] : lm::cursor{txn}) {
        const std::vector<uint8_t> key1{key.data, key.data + key.size};
        const std::vector<uint8_t> key2 = it->first;

        const std::vector<uint8_t> value1{value.data, value.data + value.size};
        const std::vector<uint8_t> value2 = it->second;

        if (key1 != key2 || value1 != value2) {
            return 1;
        }

        ++it;
    }

    return 0;
}