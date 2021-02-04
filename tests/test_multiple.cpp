//
//  test_multiple.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 01.02.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <unordered_map>
#include <vector>

#include <lmdbw/dbw.h>

int main() {
    struct item {
        uint16_t w;
        uint16_t h;
        std::vector<uint8_t> data;

        bool operator==(const item &rhs) const {
            return w == rhs.w && h == rhs.h && data == rhs.data;
        }
    };

    const std::vector<uint32_t> keys{100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    const std::vector<item> items{
            {8,  10,  {1,  2,  3,  4,  5,  6,  7,  8}},
            {10, 20,  {11, 21, 31, 41, 51, 61, 71, 81}},
            {12, 30,  {12, 22, 32, 42, 52, 62, 72, 82}},
            {14, 40,  {13, 23, 33, 43, 53, 63, 73, 83}},
            {16, 50,  {14, 24, 34, 44}},
            {18, 60,  {15, 25, 35, 45, 55, 65}},
            {20, 70,  {16, 26}},
            {22, 80,  {17, 27, 37, 47, 57}},
            {24, 90,  {18, 28, 38, 48, 58, 68, 78, 88}},
            {26, 100, {}}
    };

    //

    lm::dbw<uint32_t, item> db({"./db.db", lm::flag::env::no_subdir},
                               {"multiple", lm::flag::dbi::create | lm::flag::dbi::reverse_key}, {
                                       &item::w, &item::h, &item::data});

    // Put

    std::unordered_map<uint32_t, item> source_map;
    for (size_t i = 0; i < keys.size(); ++i) {
        source_map.emplace(keys[i], items[i]);
    }

    db.put(source_map);

    // Get

    const auto get_map = db.get(190, 850);

    //

    source_map.erase(100);
    source_map.erase(900);
    source_map.erase(1000);

    return (source_map == get_map) ? 0 : 1;
}