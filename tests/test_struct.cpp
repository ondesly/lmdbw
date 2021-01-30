//
//  test_struct.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 30.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <iostream>
#include <vector>

#include <lmdbw/dbw.h>

int main() {
    struct item {
        uint8_t u1;
        uint16_t u2;
        uint32_t u3;
        uint64_t u4;
        std::vector<uint8_t> v;

        bool operator==(const item &i) const {
            return u1 == i.u1 && u2 == i.u2 && u3 == i.u3 && u4 == i.u4 && v == i.v;
        }

    };

    const uint32_t key_1 = 11;
    const item item_1{4, 8, 16, 32, {'t', 'e', 's', 't', '_', 'd', 'a', 't', 'a', '_', '1'}};
    const item item_2{40, 80, 160, 320, {'d', 'a', 't', 'a', '_', '2'}};

    //

    lm::dbw<uint32_t, item> db({"./db.db", lm::flag::env::no_subdir},
                               {"struct", lm::flag::dbi::create}, {
                                       &item::u1, &item::u2, &item::u3, &item::u4, &item::v});

    // Create

    db.put(key_1, item_1);

    // Read

    const auto create_item = db.get(key_1);

    // Update

    db.put(key_1, item_2);
    const auto update_item = db.get(key_1);

    // Delete

    db.del(key_1);
    const auto delete_item = db.get(key_1);

    //

    return (create_item == item_1 && update_item == item_2 && delete_item == item{}) ? 0 : 1;
}