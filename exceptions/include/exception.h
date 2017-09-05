//
// Created by boa on 05.09.17.
//
#ifndef S5P_EXCEPTION_HPP
#define S5P_EXCEPTION_HPP

#include <boost/system/system_error.hpp>

#include <exception>
#include "../../service/include/service.h"


namespace socks5_proxy {
    namespace exceptions {
        using namespace service;

        class BasicError : public std::exception {
        public:
            BasicError();
        };


        class BasicBoostError : public BasicError {
        public:
            explicit BasicBoostError(b_sys::system_error &&systemError);

            const b_sys::error_code &code() const;

            const char *what() const noexcept override;

        private:
            b_sys::system_error systemError_;
        };


        class ResolutionError : public BasicBoostError {
        public:
            explicit ResolutionError(b_sys::system_error &&systemError);
        };


        class ConnectionError : public BasicBoostError {
        public:
            explicit ConnectionError(b_sys::system_error &&systemError);
        };


        class BasicPlainError : public BasicError {
        public:
            explicit BasicPlainError(const std::string &message);

            const char *what() const noexcept override;

        private:
            std::string message_;
        };


        class Socks5Error : public BasicPlainError {
        public:
            explicit Socks5Error(const std::string &message);
        };


        class EndOfFileError : public BasicError {
        };

    }


}

#endif
