//
// Created by boa on 05.09.17.
//

#ifndef S5P_SERVER_HPP
#define S5P_SERVER_HPP

#include "../../application/include/application.h"

#include <memory>


namespace socks5_proxy {
    using namespace service;
    namespace server {
        class Server {
        public:
            explicit Server(IOLoop &loop);

            void listenV4(uint16_t port);

            void listenV6(uint16_t port);


            Server(const Server &) = delete;

            Server &operator=(const Server &) = delete;

            Server(Server &&) = delete;

            Server &operator=(Server &&)= delete;

        private:
            class Private;

            std::shared_ptr<Private> pServer_;
        };

    }

}

#endif
