//
// Created by boa on 05.09.17.
//

#include "../include/application_p.h"

#include <boost/asio/signal_set.hpp>

#include <iostream>
#include <sstream>
#include <cassert>
#include <csignal>
#include <endian.h>


namespace socks5_proxy {
    namespace application {

/*
        using socks5_proxy::Application;
        using socks5_proxy::IOLoop;
        using socks5_proxy::Options;
        using socks5_proxy::OptionMap;
        using socks5_proxy::ErrorCode;
        using socks5_proxy::AddressType;
        using socks5_proxy::AddressV4;
        using socks5_proxy::AddressV6;
*/


        static Application *singleton = nullptr;
        using namespace socks5_proxy;
        using namespace service;

        Application &Application::instance() {
            return *singleton;
        }


        Application::Application(int argc, char **argv)
                : pApplication_(std::make_shared<Private>(argc, argv)) {
            assert(!singleton || !"do not create Application again");
            singleton = this;
        }

        int Application::prepare() {
            auto options = pApplication_->createOptions();
            OptionMap args;
            try {
                args = pApplication_->parseOptions(options);
            } catch (std::exception &e) {
                reportError("invalid argument", e);
                return 1;
            }

            if (args.empty() || args.count("help") >= 1) {
                std::cout << options << std::endl;
                return -1;
            }

            std::ostringstream os;
            if (this->getPort() == 0) {
                os << "missing <port>" << std::endl;
            }
            if (this->getSocks5Host().empty()) {
                os << "missing <socks5_host_>" << std::endl;
            }
            if (this->getSocks5Port() == 0) {
                os << "missing <socks5_port>" << std::endl;
            }
            if (this->getHttpPort() == 0) {
                os << "missing <http_port_>" << std::endl;
            }
            if (this->getHttpHostType() == AddressType::UNKNOWN) {
                os << "invalid <http_host>" << std::endl;
            }
            auto error_string = os.str();
            if (!error_string.empty()) {
                reportError(error_string);
                return 1;
            }

            return 0;
        }

        IOLoop &Application::ioLoop() const {
            return pApplication_->loop_;
        }

        uint16_t Application::getPort() const {
            return pApplication_->port;
        }

        const std::string &Application::getSocks5Host() const {
            return pApplication_->socks5Host_;
        }

        uint16_t Application::getSocks5Port() const {
            return pApplication_->socks5Port_;
        }

        uint16_t Application::getHttpPort() const {
            return pApplication_->httpPort_;
        }

        AddressType Application::getHttpHostType() const {
            return pApplication_->httpHostType_;
        }

        const AddressV4 &Application::getHttpHostAsIpv4() const {
            return pApplication_->httpHostIpv4_;
        }

        const AddressV6 &Application::getHttpHostAsIpv6() const {
            return pApplication_->httpHostIpv6_;
        }

        const std::string &Application::getHttpHostAsFqdn() const {
            return pApplication_->httpHostFqdn_;
        }

        int Application::exec() {
            namespace ph = std::placeholders;

            socks5_proxy::SignalHandler signals(pApplication_->loop_, SIGINT, SIGTERM);
            signals.async_wait(std::bind(&Application::Private::onSystemSignal, pApplication_, ph::_1, ph::_2));

            pApplication_->loop_.run();
            return 0;
        }

        Application::Private::Private(int argc, char **argv)
                : loop_(), argc_(argc), argv_(argv), port(0), socks5Host_(), socks5Port_(0), httpPort_(0),
                  httpHostType_(AddressType::UNKNOWN), httpHostFqdn_() {
        }

        Options Application::Private::createOptions() {

            Options od("SOCKS5 proxy");
            od.add_options()
                    ("help,h", "show this message")
                    ("port,p", po::value<uint16_t>()
                             ->value_name("<port>")
                             ->notifier(std::bind(&Application::Private::setPort, this, ph::_1)),
                     "listen to the port")
                    ("socks5-host", po::value<std::string>()
                            ->value_name("<socks5_host_>")
                            ->notifier(std::bind(&Application::Private::setSocks5Host, this, ph::_1)), "SOCKS5 host")
                    ("socks5-port", po::value<uint16_t>()
                            ->value_name("<socks5_port>")
                            ->notifier(std::bind(&Application::Private::setSocks5Port, this, ph::_1)), "SOCKS5 port")
                    ("http-host", po::value<std::string>()
                             ->value_name("<http_host>")
                             ->notifier(std::bind(&Application::Private::setHttpHost, this, ph::_1)),
                     "forward to this host")
                    ("http-port", po::value<uint16_t>()
                             ->value_name("<http_port_>")
                             ->notifier(std::bind(&Application::Private::setHttpPort, this, ph::_1)),
                     "forward to this port");
            return std::move(od);
        }

        OptionMap Application::Private::parseOptions(const Options &options) const {


            OptionMap vm;
            auto rv = po::parse_command_line(argc_, argv_, options);
            po::store(rv, vm);
            po::notify(vm);

            return std::move(vm);
        }

        void Application::Private::onSystemSignal(const ErrorCode &ec, int signalNumber) {
            if (ec) {
                reportError("signal", ec);
            }
            std::cout << "received " << signalNumber << std::endl;
            this->loop_.stop();
        }

        void Application::Private::setPort(uint16_t port) {
            this->port = port;
        }

        void Application::Private::setSocks5Host(const std::string &socks5_host) {
            this->socks5Host_ = socks5_host;
        }

        void Application::Private::setSocks5Port(uint16_t socks5_port) {
            this->socks5Port_ = socks5_port;
        }

        void Application::Private::setHttpHost(const std::string &http_host) {
            ErrorCode ec;
            auto address = Address::from_string(http_host, ec);
            if (ec) {
                this->httpHostType_ = AddressType::MAGIC;
                this->httpHostFqdn_ = http_host;
            } else if (address.is_v4()) {
                this->httpHostType_ = AddressType::IPV4;
                this->httpHostIpv4_ = address.to_v4();
            } else if (address.is_v6()) {
                this->httpHostType_ = AddressType::IPV6;
                this->httpHostIpv6_ = address.to_v6();
            } else {
                this->httpHostType_ = AddressType::UNKNOWN;
            }
        }

        void Application::Private::setHttpPort(uint16_t http_port) {
            this->httpPort_ = http_port;
        }

        Chunk createChunk() {
            return Chunk();
        }

        void putBigEndian(uint8_t *dst, uint16_t native) {
            auto view = reinterpret_cast<uint16_t *>(dst);
            *view = htobe16(native);
        }

        void reportError(const std::string &message) {
            std::cerr << message << std::endl;
        }

        void reportError(const std::string &message, const b_sys::error_code &errorCode) {
            std::cerr << message << " (code: " << errorCode << ", what: " << errorCode.message() << ")" << std::endl;
        }

        void reportError(const std::string &message, const BasicBoostError &basicBoostError) {
            std::cerr << message << " (code: " << basicBoostError.code() << ", what: " << basicBoostError.what() << ")"
                      << std::endl;
        }

        void reportError(const std::string &message, const std::exception &e) {
            std::cerr << message << " (what: " << e.what() << ")" << std::endl;
        }


    }

}


