#pragma once
#include "httplib.hpp"
#include "udsf.hpp"

void registerHandlers(httplib::Server &server, UDSF &udsf);
