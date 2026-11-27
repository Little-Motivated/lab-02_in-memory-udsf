#pragma once
#include "httplib.hpp"
#include "udsf.hpp"

class UDSFServer {
public:
    void run(std::string host, unsigned int port);

private:
    UDSF udsf;
    httplib::Server server;
    void registerHandlers(httplib::Server &server, UDSF &udsf);
};
