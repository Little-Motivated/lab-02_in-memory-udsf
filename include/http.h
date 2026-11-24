#pragma once
#include "httplib.h"
#include "udsf.h"

void registerHandlers(httplib::Server &server, UDSF &udsf);
