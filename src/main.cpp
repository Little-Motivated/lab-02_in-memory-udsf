#include "http.h"
#include "httplib.h"
#include "udsf.h"

#include <iostream>

int main() {
    UDSF udsf;
    httplib::Server server;
    registerHandlers(server, udsf);

    server.listen("0.0.0.0", 8080);
}
