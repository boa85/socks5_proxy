//
// Created by boa on 05.09.17.
//

#include "../include/session_p.h"

#include "../../application/include/application.h"
#include "../../exceptions/include/exception.h"

#include <boost/lexical_cast.hpp>

using namespace socks5_proxy;
using namespace application;
namespace socks5_proxy {
    namespace session {
        using namespace service;

        Session::Session(Socket socket)
                : pSession_(std::make_shared<Session::Private>(std::move(socket))) {
        }

        void Session::start() {
            namespace ph = std::placeholders;
            pSession_->self_ = this->shared_from_this();
            boost::asio::spawn(pSession_->loop_, std::bind(&Session::Private::doStart, pSession_, ph::_1));
        }

        void Session::stop() {
            try {
                pSession_->innerSocket_.shutdown(Socket::shutdown_both);
            } catch (std::exception &e) {
                reportError("inner socket_ shutdown failed", e);
            }
            try {
                pSession_->innerSocket_.close();
            } catch (std::exception &e) {
                reportError("inner socket_ close failed", e);
            }
            try {
                pSession_->outerSocket_.shutdown(Socket::shutdown_both);
            } catch (std::exception &e) {
                reportError("outer socket_ shutdown failed", e);
            }
            try {
                pSession_->outerSocket_.close();
            } catch (std::exception &e) {
                reportError("outer socket_ close failed", e);
            }
        }


        Session::Private::Private(Socket socket)
                : self_(), outerSocket_(std::move(socket)), loop_(this->outerSocket_.get_io_service()),
                  innerSocket_(this->loop_) {
        }

        std::shared_ptr<Session> Session::Private::magicFunction() {
            return this->self_.lock();
        }

        void Session::Private::doStart(YieldContext yield) {
            auto self = this->magicFunction();
            bool ok = false;

            try {
                auto resolvedRange = this->doInnerResolve(yield);
                for (auto it = resolvedRange.first; !ok && it != resolvedRange.second; ++it) {
                    ok = this->doInnerConnect(yield, it);
                }
                if (!ok) {
                    reportError("no resolved address is available");
                    return;
                }
            } catch (ResolutionError &e) {
                reportError("cannot resolve the domain", e);
                return;
            }

            try {
                this->doInnerSocks5(yield);
            } catch (EndOfFileError &) {
                self->stop();
                return;
            } catch (Socks5Error &e) {
                reportError("socks5 auth error", e);
                return;
            } catch (ConnectionError &e) {
                reportError("socks5 connection error", e);
                return;
            }


            boost::asio::spawn(this->loop_, [this](YieldContext yield) -> void {
                this->doProxyng(yield, this->outerSocket_, this->innerSocket_);
            });
            boost::asio::spawn(this->loop_, [this](YieldContext yield) -> void {
                this->doProxyng(yield, this->innerSocket_, this->outerSocket_);
            });
        }

        std::size_t Session::Private::doRead(YieldContext yield, Socket &socket, Chunk &chunk) {
            auto buffer = boost::asio::buffer(chunk);
            try {
                auto length = socket.async_read_some(buffer, yield);
                return length;
            } catch (b_sys::system_error &e) {
                if (e.code() == ba::error::eof) {
                    throw EndOfFileError();
                } else {
                    throw ConnectionError(std::move(e));
                }
            }
        }

        void Session::Private::doWrite(YieldContext yield, Socket &socket, const Chunk &chunk, std::size_t length) {
            try {
                std::size_t offset = 0;
                while (length > 0) {
                    auto buffer = boost::asio::buffer(&chunk[offset], length);
                    auto wroteLength = socket.async_write_some(buffer, yield);
                    offset += wroteLength;
                    length -= wroteLength;
                }
            } catch (b_sys::system_error &e) {
                throw ConnectionError(std::move(e));
            }
        }

        ResolvedRange Session::Private::doInnerResolve(YieldContext yield) {
            Resolver resolver(this->loop_);
            auto host = Application::instance().getSocks5Host();
            auto port = boost::lexical_cast<std::string>(Application::instance().getSocks5Port());

            try {
                auto it = resolver.async_resolve({host, port,}, yield);
                return {it, Resolver::iterator()};
            } catch (b_sys::system_error &e) {
                throw ResolutionError(std::move(e));
            }

            return {Resolver::iterator(), Resolver::iterator()};
        }

        bool Session::Private::doInnerConnect(YieldContext yield, Resolver::iterator it) {
            if (Resolver::iterator() == it) {
                return false;
            }

            try {
                this->innerSocket_.async_connect(*it, yield);
            } catch (b_sys::system_error &e) {
                this->innerSocket_.close();
                return false;
            }

            return true;
        }

        void Session::Private::doInnerSocks5(YieldContext yield) {
            this->doInnerSocks5Phase1(yield);
            this->doInnerSocks5Phase2(yield);
        }

        void Session::Private::doInnerSocks5Phase1(YieldContext yield) {
            auto chunk = createChunk();
            // VER
            chunk[0] = 0x05;
            // NMETHODS
            chunk[1] = 0x01;
            // METHODS
            chunk[2] = 0x00;

            this->doWrite(yield, this->innerSocket_, chunk, 3);
            auto length = this->doRead(yield, this->innerSocket_, chunk);

            if (length < 2) {
                throw Socks5Error("wrong auth header length");
            }
            if (chunk[1] != 0x00) {
                throw Socks5Error("provided auth not supported");
            }
        }

        std::size_t fill_ipv4(socks5_proxy::Chunk &buffer, std::size_t offset) {
            // ATYP
            buffer[offset++] = 0x01;

            // DST.ADDR
            auto bytes = Application::instance().getHttpHostAsIpv4().to_bytes();

            std::copy_n(std::begin(bytes), bytes.size(), std::next(std::begin(buffer), offset));

            return 1 + bytes.size();
        }

        std::size_t fill_ipv6(socks5_proxy::Chunk &buffer, std::size_t offset) {
            // ATYP
            buffer[offset++] = 0x04;

            // DST.ADDR
            auto bytes = Application::instance().getHttpHostAsIpv6().to_bytes();

            std::copy_n(std::begin(bytes), bytes.size(), std::next(std::begin(buffer), offset));

            return 1 + bytes.size();
        }

        std::size_t fill_magic(socks5_proxy::Chunk &buffer, std::size_t offset) {
            // ATYP
            buffer[offset++] = 0x03;

            // DST.ADDR
            const std::string &hostname = Application::instance().getHttpHostAsFqdn();

            buffer[offset++] = static_cast<uint8_t>(hostname.size());
            std::copy(std::begin(hostname), std::end(hostname), std::next(std::begin(buffer), offset));

            return 1 + 1 + hostname.size();
        }

        void Session::Private::doInnerSocks5Phase2(YieldContext yield) {
            auto chunk = createChunk();
            // VER
            chunk[0] = 0x05;
            // CMD
            chunk[1] = 0x01;
            // RSV
            chunk[2] = 0x00;

            std::size_t used_byte = 0;
            switch (Application::instance().getHttpHostType()) {
                case AddressType::IPV4:
                    used_byte = fill_ipv4(chunk, 3);
                    break;
                case AddressType::IPV6:
                    used_byte = fill_ipv6(chunk, 3);
                    break;
                case AddressType::MAGIC:
                    used_byte = fill_magic(chunk, 3);
                    break;
                default:
                    throw Socks5Error("unknown target http address");
            }

            // DST.PORT
            putBigEndian(&chunk[3 + used_byte], Application::instance().getHttpPort());

            std::size_t total_length = 3 + used_byte + 2;

            this->doWrite(yield, this->innerSocket_, chunk, total_length);
            auto length = this->doRead(yield, this->innerSocket_, chunk);

            if (length < 3) {
                throw Socks5Error("server replied error");
            }
            if (chunk[1] != 0x00) {
                throw Socks5Error("server replied error");
            }
            switch (chunk[3]) {
                case 0x01:
                    break;
                case 0x03:
                    break;
                case 0x04:
                    break;
                default:
                    throw Socks5Error("unknown address type");
            }
        }

        void Session::Private::doProxyng(YieldContext yield, Socket &input, Socket &output) {
            auto self = this->magicFunction();
            auto chunk = createChunk();
            try {
                while (true) {
                    auto length = this->doRead(yield, input, chunk);

                    this->doWrite(yield, output, chunk, length);
                }
            } catch (EndOfFileError &e) {
                self->stop();
            } catch (ConnectionError &e) {
                reportError("connection error", e);
            }

        }

    }//namespace session


}//namespace socks5_proxy
