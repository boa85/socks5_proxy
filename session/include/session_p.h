//
// Created by boa on 05.09.17.
//


#ifndef S5P_SESSION_HPP_
#define S5P_SESSION_HPP_

#include "session.h"

#include <boost/asio/spawn.hpp>

#include <memory>


namespace socks5_proxy {
    namespace session {
        class Session::Private {
        public:
            Private(Socket socket);

            std::shared_ptr<Session> magicFunction();

            void doStart(YieldContext yield);

            ResolvedRange doInnerResolve(YieldContext yield);

            bool doInnerConnect(YieldContext yield, Resolver::iterator it);

            void doInnerSocks5(YieldContext yield);

            void doInnerSocks5Phase1(YieldContext yield);

            void doInnerSocks5Phase2(YieldContext yield);

            void doProxyng(YieldContext yield, Socket &input, Socket &output);

            void doWrite(YieldContext yield, Socket &socket, const Chunk &chunk, std::size_t length);

            std::size_t doRead(YieldContext yield, Socket &socket, Chunk &chunk);

            std::weak_ptr<Session> self_;
            Socket outerSocket_;
            IOLoop &loop_;
            Socket innerSocket_;
        };

    }


}

#endif
