//
// Created by boa on 05.09.17.
//

#include "../include/server_p.h"

#include "../../session/include/session.h"

#include <boost/asio/ip/v6_only.hpp>

namespace socks5_proxy {
    namespace server {
        using namespace application;


        Server::Server(IOLoop &loop)
                : pServer_(std::make_shared<Server::Private>(loop)) {
        }

        void Server::listenV4(uint16_t port) {
            pServer_->doV4Listen(port);
            pServer_->doV4Accept();
        }

        void Server::listenV6(uint16_t port) {
            pServer_->doV6Listen(port);
            pServer_->doV6Accept();
        }

        Server::Private::Private(IOLoop &loop)
                : v4Acceptor_(loop), v6Acceptor_(loop), socket_(loop) {
        }

        void Server::Private::doV4Listen(uint16_t port) {
            EndPoint ep(ba::ip::tcp::v4(), port);
            this->v4Acceptor_.open(ep.protocol());
            this->v4Acceptor_.set_option(Acceptor::reuse_address(true));
            this->v4Acceptor_.bind(ep);
            this->v4Acceptor_.listen();
        }

        void Server::Private::doV4Accept() {
            v4Acceptor_.async_accept(socket_, [this](const ErrorCode &ec) -> void {
                if (ec) {
                    reportError("do V4 accept", ec);
                } else {
                    std::make_shared<Session>(std::move(socket_))->start();
                }

                doV4Accept();
            });
        }

        void Server::Private::doV6Listen(uint16_t port) {
            EndPoint ep(ba::ip::tcp::v6(), port);
            v6Acceptor_.open(ep.protocol());
            v6Acceptor_.set_option(Acceptor::reuse_address(true));
            v6Acceptor_.set_option(ba::ip::v6_only(true));
            v6Acceptor_.bind(ep);
            v6Acceptor_.listen();
        }

        void Server::Private::doV6Accept() {
            this->v6Acceptor_.async_accept(socket_, [this](const ErrorCode &ec) -> void {
                if (ec) {
                    reportError("doV6Accept", ec);
                } else {
                    std::make_shared<Session>(std::move(socket_))->start();
                }

                this->doV6Accept();
            });
        }

    }
}

