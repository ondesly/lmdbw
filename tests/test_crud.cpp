//
//  test_crud.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <vector>

#include <lmdbw/db.h>
#include <lmdbw/transaction.h>

int main() {
    const std::vector<uint8_t> key = {'t', 'e', 's', 't', '_', 'k', 'e', 'y'};
    const std::vector<uint8_t> value_1 = {'t', 'e', 's', 't', '_', 'v', 'a', 'l', 'u', 'e', '_', '1'};
    const std::vector<uint8_t> value_2 = {'v', 'a', 'l', 'u', 'e', '_', '2'};

    //

    lm::val create_value{};
    lm::val update_value{};
    lm::val delete_value{};

    lm::db db(
            {"./db.db", lm::flag::env::no_subdir},
            {"test", lm::flag::dbi::create});

    // Create

    {
        lm::transaction txn(db);
        txn.put({key.data(), key.size()}, {value_1.data(), value_1.size()});
    }

    // Read

    {
        lm::transaction txn(db, lm::flag::transaction::rd_only);
        create_value = txn.get({key.data(), key.size()});
    }

    // Update

    {
        lm::transaction txn(db);
        txn.put({key.data(), key.size()}, {value_2.data(), value_2.size()});
    }

    {
        lm::transaction txn(db, lm::flag::transaction::rd_only);
        update_value = txn.get({key.data(), key.size()});
    }

    // Delete

    {
        lm::transaction txn(db);
        txn.del({key.data(), key.size()});
    }

    {
        lm::transaction txn(db, lm::flag::transaction::rd_only);
        delete_value = txn.get({key.data(), key.size()});
    }

    return (std::vector<uint8_t>(create_value.data, create_value.data + create_value.size) == value_1 &&
            std::vector<uint8_t>(update_value.data, update_value.data + update_value.size) == value_2 &&
            std::vector<uint8_t>(delete_value.data, delete_value.data + delete_value.size).empty()) ? 0 : 1;
}