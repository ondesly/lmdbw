//
//  serializer.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 30.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "lmdbw/column.h"
#include "lmdbw/val.h"

namespace lm {

    template<class T>
    class serializer;

}

namespace {

    template<class T>
    lm::val encode(const T &value, lm::serializer<T> &ser);

    lm::val encode(const std::string &value, lm::serializer<std::string> &ser);

    template<class T>
    T decode(const lm::val &data, lm::serializer<T> &ser);

    std::string decode(const lm::val &data, lm::serializer<std::string> &ser);

}

namespace lm {

    using vector_size_t = uint32_t;

    template<class T>
    class serializer {
    public:

        explicit serializer(const std::vector<lm::column<T>> &columns) : m_columns(columns) {}

    public:

        lm::val encode(const T &value) {
            return ::encode(value, *this);
        }

        lm::val encode_struct(const T &value) {
            auto size = get_size(value);
            m_buffer = std::make_unique<uint8_t[]>(size);

            size_t index = 0;
            for (const auto &c : m_columns) {
                switch (c.get_type()) {
                    case column<T>::type::uint8:
                        copy(value.*(c.uint8_field()), m_buffer.get(), index);
                        break;
                    case column<T>::type::uint16:
                        copy(value.*(c.uint16_field()), m_buffer.get(), index);
                        break;
                    case column<T>::type::uint32:
                        copy(value.*(c.uint32_field()), m_buffer.get(), index);
                        break;
                    case column<T>::type::uint64:
                        copy(value.*(c.uint64_field()), m_buffer.get(), index);
                        break;
                    case column<T>::type::vector8:
                        copy(value.*(c.vector8_field()), m_buffer.get(), index);
                        break;
                    case column<T>::type::vector32:
                        copy(value.*(c.vector32_field()), m_buffer.get(), index);
                        break;
                }
            }

            return {m_buffer.get(), size};
        }

        lm::val encode_string(const std::string &value) {
            return {reinterpret_cast<const uint8_t *>(value.data()), value.size()};
        }

        T decode(const lm::val &data) {
            return ::decode(data, *this);
        }

        T decode_struct(const lm::val &data) {
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
                    case column<T>::type::vector8:
                        copy(data.data, index, value.*(c.vector8_field()));
                        break;
                    case column<T>::type::vector32:
                        copy(data.data, index, value.*(c.vector32_field()));
                        break;
                }
            }

            return value;
        }

        std::string decode_string(const lm::val &data) {
            return {data.data, data.data + data.size};
        }

    private:

        const std::vector<lm::column<T>> &m_columns;

        std::unique_ptr<uint8_t[]> m_buffer;

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

        template<class C>
        inline void copy(const std::vector<C> &src, uint8_t *dst, size_t &index) {
            const auto vector_size = vector_size_t(src.size());
            copy(vector_size, dst, index);

            const auto ptr = reinterpret_cast<const uint8_t *>(src.data());
            std::copy_n(ptr, vector_size * sizeof(C), dst + index);
            index += vector_size * sizeof(C);
        }

        template<class C>
        inline void copy(const uint8_t *src, size_t &index, std::vector<C> &dst) {
            vector_size_t vector_size;
            copy(src, index, vector_size);

            dst.resize(vector_size);

            const auto ptr = reinterpret_cast<uint8_t *>(dst.data());
            std::copy_n(src + index, vector_size * sizeof(C), ptr);
            index += vector_size * sizeof(C);
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
                    case column<T>::type::vector8:
                        size += sizeof(vector_size_t) + (value.*(c.vector8_field())).size() * sizeof(uint8_t);
                        break;
                    case column<T>::type::vector32:
                        size += sizeof(vector_size_t) + (value.*(c.vector32_field())).size() * sizeof(uint32_t);
                        break;
                }
            }

            return size;
        }

    };

}

namespace {

    template<class T>
    lm::val encode(const T &value, lm::serializer<T> &ser) {
        return ser.encode_struct(value);
    }

    lm::val encode(const std::string &value, lm::serializer<std::string> &ser) {
        return ser.encode_string(value);
    }

    template<class T>
    T decode(const lm::val &data, lm::serializer<T> &ser) {
        return ser.decode_struct(data);
    }

    std::string decode(const lm::val &data, lm::serializer<std::string> &ser) {
        return ser.decode_string(data);
    }

}