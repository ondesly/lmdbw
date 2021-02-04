//
//  val.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 30.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <algorithm>

namespace lm {

    struct val {

        const uint8_t *data;
        size_t size;

        static inline lm::val copy(const lm::val &value) {
            const auto data = new uint8_t[value.size];
            std::copy_n(value.data, value.size, data);
            return {data, value.size};
        }

    };

    template<class V>
    inline V to(const lm::val &value) {
        return {value.size, const_cast<void *>(static_cast<const void *>(value.data))};
    }

    template<class V>
    inline lm::val to_val(const V &value) {
        return {static_cast<uint8_t *>(value.mv_data), value.mv_size};
    }

}