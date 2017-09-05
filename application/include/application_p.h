//
// Created by boa on 05.09.17.
//
#ifndef S5P_GLOBAL_HPP_
#define S5P_GLOBAL_HPP_


#include "application.h"

#include <boost/program_options.hpp>


namespace socks5_proxy {
    namespace application {
        class Application::Private {
        public:
            Private(int argc, char **argv);

            Options createOptions();

            OptionMap parseOptions(const Options &options) const;

            void onSystemSignal(const ErrorCode &ec, int signalNumber);

            void setPort(uint16_t port);

            void setSocks5Host(const std::string &host);

            void setSocks5Port(uint16_t port);

            void setHttpHost(const std::string &host);

            void setHttpPort(uint16_t port);

            IOLoop loop_;
            int argc_;
            char **argv_;
            uint16_t port;
            std::string socks5Host_;
            uint16_t socks5Port_;
            uint16_t httpPort_;
            AddressType httpHostType_;
            AddressV4 httpHostIpv4_;
            AddressV6 httpHostIpv6_;
            std::string httpHostFqdn_;
        };

    }


}


#endif
