//
//  exception.h
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <exception>
#include <string>

namespace lm {

    class exception : public std::exception {
    public:

        explicit exception(const std::string &message, int code)
                : m_message(message + " failed (" + std::to_string(code) + ")") {}

        const char *what() const noexcept override {
            return m_message.c_str();
        }

    private:

        const std::string m_message;

    };

}