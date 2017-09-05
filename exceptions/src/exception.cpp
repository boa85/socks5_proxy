//
// Created by boa on 05.09.17.
//
#include "../include/exception.h"

namespace socks5_proxy {
    namespace exceptions {

        BasicError::BasicError()
                : std::exception() {}

        BasicBoostError::BasicBoostError(b_sys::system_error &&systemError)
                : systemError_(systemError) {

        }

        const b_sys::error_code &BasicBoostError::code() const {
            return this->systemError_.code();
        }

        const char *BasicBoostError::what() const noexcept {
            return this->systemError_.what();
        }

        ResolutionError::ResolutionError(b_sys::system_error &&systemError)
                : BasicBoostError(std::move(systemError)) {

        }

        ConnectionError::ConnectionError(b_sys::system_error &&systemError)
                : BasicBoostError(std::move(systemError)) {

        }

        BasicPlainError::BasicPlainError(const std::string &message)
                : BasicError(), message_(message) {

        }

        const char *BasicPlainError::what() const noexcept {
            return this->message_.c_str();
        }

        Socks5Error::Socks5Error(const std::string &message)
                : BasicPlainError(message) {}

    }
}


