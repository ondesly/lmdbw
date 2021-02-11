//
//  pack.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 11.02.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include "lmdbw/val.h"

namespace lm {

    template<class ... Types>
    class pack {
    public:

        pack(Types... args) {
            fill(m_buffer, args...);
        }

        pack(const uint8_t *buf) {
            std::copy_n(buf, get_size(), m_buffer);
        }

        std::tuple<Types...> get() const {
            if constexpr (sizeof...(Types) == 3) {
                using t0 = typename std::tuple_element<0, std::tuple<Types...>>::type;
                using t1 = typename std::tuple_element<1, std::tuple<Types...>>::type;
                using t2 = typename std::tuple_element<2, std::tuple<Types...>>::type;

                return {*reinterpret_cast<const t0 *>(m_buffer),
                        *reinterpret_cast<const t1 *>(m_buffer + sizeof(t0)),
                        *reinterpret_cast<const t2 *>(m_buffer + sizeof(t0) + sizeof(t1))};
            } else if constexpr (sizeof...(Types) == 2) {
                using t0 = typename std::tuple_element<0, std::tuple<Types...>>::type;
                using t1 = typename std::tuple_element<1, std::tuple<Types...>>::type;

                return {*reinterpret_cast<const t0 *>(m_buffer),
                        *reinterpret_cast<const t1 *>(m_buffer + sizeof(t0))};
            } else if constexpr (sizeof...(Types) == 1) {
                using t0 = typename std::tuple_element<0, std::tuple<Types...>>::type;

                return {*reinterpret_cast<const t0 *>(m_buffer)};
            } else {
                return {};
            }
        }

        operator val() const {
            return {m_buffer, get_size()};
        }

    private:

        static void fill(uint8_t *) {}

        template<class T, class... Ts>
        static void fill(uint8_t *buf, T &arg, Ts &... args) {
            reinterpret_cast<T *>(buf)[0] = arg;
            fill(buf + sizeof(T), args...);
        }

        static constexpr auto get_size() {
            size_t sum = 0;
            for (auto size : {sizeof(Types)...}) {
                sum += size;
            }
            return sum;
        }

        static constexpr auto get_pos(size_t index) {
            const auto sizes = {sizeof(Types)...};
            size_t pos = 0;
            for (size_t i = 0; i < index; ++i) {
                pos += sizes[i];
            }
            return pos;
        }

    private:

        uint8_t m_buffer[get_size()];

    };

}
