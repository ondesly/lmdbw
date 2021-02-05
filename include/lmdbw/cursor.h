//
//  cursor.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 03.02.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <iterator>
#include <utility>

#include "lmdbw/val.h"

struct MDB_cursor;

namespace lm {

    class transaction;

    class cursor {
    public:

        class iterator {

            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::pair<lm::val, lm::val>;
            using pointer = std::pair<lm::val, lm::val> *;
            using reference = std::pair<lm::val, lm::val> &;

        public:

            iterator(lm::cursor &cursor, const lm::val &key);

        public:

            reference operator*() const;

            iterator &operator++();

            void del();

            bool operator==(const iterator &other) const;

            bool operator!=(const iterator &other) const;

        private:

            lm::cursor &m_cursor;
            const lm::val &m_key;

        };

    public:

        cursor(const transaction &transaction, const lm::val &begin, const lm::val &end);

        ~cursor();

    public:

        iterator begin();

        iterator end();

    private:

        const transaction &m_transaction;
        MDB_cursor *m_cursor{};

        std::pair<lm::val, lm::val> m_current;
        lm::val m_end_key;

    private:

        void set(int option, const lm::val &key = {});

        lm::val specify_end_key(const lm::val &end);

        void del();

    };

}