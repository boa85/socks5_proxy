//
// Created by boa on 05.09.17.
//

#ifndef S5P_GLOBAL_HPP
#define S5P_GLOBAL_HPP

#include "../../exceptions/include/exception.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>


namespace socks5_proxy {
    using namespace exceptions;
    using namespace service;
    namespace application {
        enum class AddressType {
            IPV4,
            IPV6,
            FQDN,
            UNKNOWN,
        };


        class Application {
        public:
            static Application &instance();

            Application(int argc, char *argv[]);

            int prepare();

            IOLoop &ioLoop() const;

            uint16_t getPort() const;

            const std::string &getSocks5Host() const;

            uint16_t getSocks5Port() const;

            const AddressV4 &getHttpHostAsIpv4() const;

            const AddressV6 &getHttpHostAsIpv6() const;

            const std::string &getHttpHostAsFqdn() const;

            uint16_t getHttpPort() const;

            AddressType getHttpHostType() const;

            int exec();


            Application(const Application &) = delete;

            Application &operator=(const Application &) = delete;

            Application(Application &&) = delete;

            Application &operator=(Application &&) = delete;

        private:
            class Private;

            std::shared_ptr<Private> pApplication_;
        };


        Chunk createChunk();

        void putBigEndian(uint8_t *dst, uint16_t native);

        void reportError(const std::string &message);

        void reportError(const std::string &message, const b_sys::error_code &errorCode);

        void reportError(const std::string &message, const socks5_proxy::BasicBoostError &basicBoostError);

        void reportError(const std::string &message, const std::exception &e);


    }

}

#endif
