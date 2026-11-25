#include "http.hpp"
#include "httplib.hpp"
#include "udsf.hpp"

#include <iostream>

int main() {
    UDSF udsf;
    httplib::Server server;
    registerHandlers(server, udsf);

    server.set_exception_handler([](const httplib::Request &req,
                                    httplib::Response &res,
                                    std::exception_ptr ep) {
        try {
            std::rethrow_exception(ep);
        } catch (const std::exception &e) {
            res.status = 500;
            res.set_content(std::string("Error: ") + e.what(), "text/plain");
        }
    });

    server.listen("0.0.0.0", 8080);
}
