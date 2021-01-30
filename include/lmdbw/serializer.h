//
//  serializer.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 30.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include "lmdbw/column.h"
#include "lmdbw/val.h"

namespace lm {

    template<class T>
    class serializer {
    public:

        explicit serializer(const std::vector<lm::column<T>> &columns) : m_columns(columns) {}

        ~serializer() {
            delete[] m_buffer;
        }

    public:

        lm::val encode(const T &value) {
            m_size = get_size(value);
            m_buffer = new uint8_t[m_size];

            size_t index = 0;
            for (const auto &c : m_columns) {
                switch (c.get_type()) {
                    case column<T>::type::uint8:
                        copy(value.*(c.uint8_field()), m_buffer, index);
                        break;
                    case column<T>::type::uint16:
                        copy(value.*(c.uint16_field()), m_buffer, index);
                        break;
                    case column<T>::type::uint32:
                        copy(value.*(c.uint32_field()), m_buffer, index);
                        break;
                    case column<T>::type::uint64:
                        copy(value.*(c.uint64_field()), m_buffer, index);
                        break;
                    case column<T>::type::vector:
                        copy(value.*(c.vector_field()), m_buffer, index);
                        break;
                }
            }

            return {m_buffer, m_size};
        }

        T decode(const lm::val &data) {
            if (!data.data) {
                return {};
            }

            T value;

            size_t index = 0;
            for (const auto &c : m_columns) {
                switch (c.get_type()) {
                    case column<T>::type::uint8:
                        copy(data.data, index, value.*(c.uint8_field()));
                        break;
                    case column<T>::type::uint16:
                        copy(data.data, index, value.*(c.uint16_field()));
                        break;
                    case column<T>::type::uint32:
                        copy(data.data, index, value.*(c.uint32_field()));
                        break;
                    case column<T>::type::uint64:
                        copy(data.data, index, value.*(c.uint64_field()));
                        break;
                    case column<T>::type::vector:
                        copy(data.data, index, value.*(c.vector_field()));
                        break;
                }
            }

            return value;
        }

    private:

        const std::vector<lm::column<T>> &m_columns;

        uint8_t *m_buffer = nullptr;
        size_t m_size = 0;

    private:

        template<class C>
        inline void copy(const C &src, uint8_t *dst, size_t &index) {
            std::copy_n(reinterpret_cast<const uint8_t *>(&src), sizeof(C), dst + index);
            index += sizeof(C);
        }

        template<class C>
        inline void copy(const uint8_t *src, size_t &index, C &dst) {
            std::copy_n(src + index, sizeof(C), reinterpret_cast<uint8_t *>(&dst));
            index += sizeof(C);
        }

        inline void copy(const std::vector<uint8_t> &src, uint8_t *dst, size_t &index) {
            const size_t vector_size = src.size();
            copy(vector_size, dst, index);

            std::copy_n(src.data(), vector_size, dst + index);
            index += sizeof(vector_size);
        }

        inline void copy(const uint8_t *src, size_t &index, std::vector<uint8_t> &dst) {
            size_t vector_size;
            copy(src, index, vector_size);

            dst.resize(vector_size);
            std::copy_n(src + index, vector_size, dst.data());
            index += sizeof(vector_size);
        }

        inline size_t get_size(const T &value) {
            size_t size = 0;

            for (const auto &c : m_columns) {
                switch (c.get_type()) {
                    case column<T>::type::uint8:
                        size += sizeof(uint8_t);
                        break;
                    case column<T>::type::uint16:
                        size += sizeof(uint16_t);
                        break;
                    case column<T>::type::uint32:
                        size += sizeof(uint32_t);
                        break;
                    case column<T>::type::uint64:
                        size += sizeof(uint64_t);
                        break;
                    case column<T>::type::vector:
                        size += sizeof(size_t) + (value.*(c.vector_field())).size();
                        break;
                }
            }

            return size;
        }

    };

}