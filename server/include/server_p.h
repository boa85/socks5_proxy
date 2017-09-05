//
// Created by boa on 05.09.17.
//

#ifndef S5P_SERVER_HPP_
#define S5P_SERVER_HPP_

#include "server.h"


namespace socks5_proxy {
    namespace server {
        class Server::Private {
        public:
            explicit Private(IOLoop &loop);

            void doV4Listen(uint16_t port);

            void doV4Accept();

            void doV6Listen(uint16_t port);

            void doV6Accept();

            Acceptor v4Acceptor_;

            Acceptor v6Acceptor_;

            Socket socket_;
        };

    }

}

#endif
