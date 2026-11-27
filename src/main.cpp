#include "server.hpp"
#include "udsf.hpp"

#include <iostream>

int main() {
    UDSFServer udsf_server;
    try {
        udsf_server.run("127.0.0.1", 8080);
    } catch (std::exception &e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}
