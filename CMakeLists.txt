cmake_minimum_required(VERSION 3.5)

project(socks5_proxy)

find_package(Boost REQUIRED COMPONENTS system program_options coroutine)
find_package(Threads REQUIRED)

set(HEADERS
        "exceptions/include/exception.h"
        "global/include/global.h"
        "global/include/global_p.h"
        "server/include/server.h"
        "server/include/server_p.h"
        "session/include/session.h"
        "session/include/session_p.h"
        "service/include/service.h")
set(SOURCES
        "exceptions/src/exception.cpp"
        "main.cpp"
        "global/src/global.cpp"
        "server/src/server.cpp"
        "session/src/session.cpp"
        "service/src/service.cpp"
        )


add_executable(socks5_proxy ${SOURCES} ${HEADERS})
target_compile_features(socks5_proxy PRIVATE cxx_auto_type)
target_compile_definitions(socks5_proxy PRIVATE BOOST_COROUTINES_NO_DEPRECATION_WARNING BOOST_COROUTINE_NO_DEPRECATION_WARNING)
target_link_libraries(socks5_proxy
    Boost::dynamic_linking
    Boost::disable_autolinking
    Boost::system
    Boost::program_options
    Boost::coroutine
    Threads::Threads)
