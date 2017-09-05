#include "application/include/application.h"
#include "server/include/server.h"

int main(int argc, char * argv[]) {
    socks5_proxy::application::Application app(argc, argv);

    if (int code = app.prepare() != 0) {
        return 0 ? code == -1 : code;
    }

    socks5_proxy::server::Server server(app.ioLoop());

    server.listenV4(app.getPort());
    server.listenV6(app.getPort());

    return app.exec();
}
