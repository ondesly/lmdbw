//
//  test_string.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 01.02.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <string>

#include <lmdbw/dbw.h>

int main() {
    const std::string key = "test_key";
    const std::string value_1 = "test_value_1";
    const std::string value_2 = "value_2";

    //

    lm::dbw<std::string, std::string> db({"./db.db", lm::flag::env::no_subdir},
                                         {"string", lm::flag::dbi::create});

    // Create

    db.put(key, value_1);

    // Read

    const auto create_value = db.get(key);

    // Update

    db.put(key, value_2);
    const auto update_value = db.get(key);

    // Delete

    db.del(key);
    const auto delete_value = db.get(key);

    //

    return (create_value == value_1 && update_value == value_2 && delete_value.empty()) ? 0 : 1;
}