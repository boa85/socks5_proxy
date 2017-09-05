//
// Created by boa on 05.09.17.
//

#ifndef S5P_SESSION_HPP
#define S5P_SESSION_HPP

#include "../../application/include/application.h"

#include <memory>


namespace socks5_proxy {
    namespace session {
        class Session : public std::enable_shared_from_this<Session> {
        public:
            explicit Session(Socket socket);

            void start();

            void stop();


            Session(const Session &) = delete;

            Session &operator=(const Session &) = delete;

            Session(Session &&) = delete;

            Session &operator=(Session &&) = delete;

        private:
            class Private;

            std::shared_ptr<Private> pSession_;
        };

    }


}

#endif
