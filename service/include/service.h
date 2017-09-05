//
// Created by boa on 05.09.17.
//

#ifndef SOCKS5_PROXY_SERVICE_H
#define SOCKS5_PROXY_SERVICE_H

#include <boost/system/system_error.hpp>


#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/program_options.hpp>
#include <boost/asio/signal_set.hpp>

namespace socks5_proxy {
    namespace service {
        namespace b_sys = boost::system;
        namespace po = boost::program_options;
        namespace ph = std::placeholders;

        using Chunk = std::array<uint8_t, 8192>;
        using IOLoop = boost::asio::io_service;
        using AddressV4 = boost::asio::ip::address_v4;
        using AddressV6 = boost::asio::ip::address_v6;
        using Acceptor = boost::asio::ip::tcp::acceptor;
        using EndPoint = boost::asio::ip::tcp::endpoint;
        using Socket = boost::asio::ip::tcp::socket;
        using ErrorCode = boost::system::error_code;
        using Options = boost::program_options::options_description;
        using OptionMap = boost::program_options::variables_map;
        using SignalHandler = boost::asio::signal_set;
        using Address = boost::asio::ip::address;

    }
}


#endif //SOCKS5_PROXY_SERVICE_H
