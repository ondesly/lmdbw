//
//  column.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 30.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <vector>

namespace lm {

    template<class T>
    class column {
    public:

        enum class type {
            uint8,
            uint16,
            uint32,
            uint64,
            vector
        };

    public:

        column(uint8_t T::* u) : m_pointer(u), m_type(type::uint8) {}

        column(uint16_t T::* u) : m_pointer(u), m_type(type::uint16) {}

        column(uint32_t T::* u) : m_pointer(u), m_type(type::uint32) {}

        column(uint64_t T::* u) : m_pointer(u), m_type(type::uint64) {}

        column(std::vector<uint8_t> T::* v) : m_pointer(v), m_type(type::vector) {}

    public:

        type get_type() const {
            return m_type;
        }

        uint8_t T::* uint8_field() const {
            return m_pointer.u8;
        }

        uint16_t T::* uint16_field() const {
            return m_pointer.u16;
        }

        uint32_t T::* uint32_field() const {
            return m_pointer.u32;
        }

        uint64_t T::* uint64_field() const {
            return m_pointer.u64;
        }

        std::vector<uint8_t> T::* vector_field() const {
            return m_pointer.v;
        }

    private:

        union pointer {

            uint8_t T::* u8;
            uint16_t T::* u16;
            uint32_t T::* u32;
            uint64_t T::* u64;
            std::vector<uint8_t> T::* v;

            explicit pointer(uint8_t T::* u8) : u8(u8) {}

            explicit pointer(uint16_t T::* u16) : u16(u16) {}

            explicit pointer(uint32_t T::* u32) : u32(u32) {}

            explicit pointer(uint64_t T::* u64) : u64(u64) {}

            explicit pointer(std::vector<uint8_t> T::* v) : v(v) {}

        };

    private:

        pointer m_pointer;
        column::type m_type;

    };

}