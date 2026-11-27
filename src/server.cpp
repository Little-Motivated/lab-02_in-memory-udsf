#include "server.hpp"
#include "httplib.hpp"
#include "nlohmann/json.hpp"
#include "udsf.hpp"

#include <optional>

namespace {

void getRecordHandler(const httplib::Request &req, httplib::Response &res,
                      UDSF &udsf) {
    std::cout << "getRecord\n";
    std::optional<Record> record = udsf.getRecord(RecordPath(
        req.path_params.at("realmId"), req.path_params.at("storageId"),
        req.path_params.at("recordId")));
    if (record == std::nullopt) {
        res.status = 404;
    } else {
        res.status = 200;
        res.set_content((*record).toJsonString(),
                        "application/json");  // get content type from record
    }
}

void createOrModifyRecordHandler(const httplib::Request &req,
                                 httplib::Response &res, UDSF &udsf) {
    std::cout << "createOrModifyRecord\n";
    Status status = DEFAULT;
    udsf.createOrModifyRecord(RecordPath(req.path_params.at("realmId"),
                                         req.path_params.at("storageId"),
                                         req.path_params.at("recordId")),
                              req.body, status);
    if (status == CREATED) {
        res.status = 201;
    } else if (status == MODIFIED) {
        res.status = 204;
    }
}

void deleteRecordHandler(const httplib::Request &req, httplib::Response &res,
                         UDSF &udsf) {
    std::cout << "deleteRecord\n";
    Status status = DEFAULT;
    udsf.deleteRecord(RecordPath(req.path_params.at("realmId"),
                                 req.path_params.at("storageId"),
                                 req.path_params.at("recordId")),
                      status);
    if (status == NOT_FOUND) {
        res.status = 404;
    } else if (status == DELETED) {
        res.status = 204;
    }
}

void getBlockListHandler(const httplib::Request &req, httplib::Response &res,
                         UDSF &udsf) {
    std::cout << "getBlockList\n";
    std::optional<Record> record = udsf.getBlockList(RecordPath(
        req.path_params.at("realmId"), req.path_params.at("storageId"),
        req.path_params.at("recordId")));
    if (record == std::nullopt) {
        res.status = 404;
    } else {
        res.status = 200;
        res.set_content((*record).toJsonStringNoMeta(), "application/json");
    }
}

void getBlockHandler(const httplib::Request &req, httplib::Response &res,
                     UDSF &udsf) {
    std::cout << "getBlock\n";
    auto block = udsf.getBlock(RecordPath(req.path_params.at("realmId"),
                                          req.path_params.at("storageId"),
                                          req.path_params.at("recordId")),
                               req.path_params.at("blockId"));
    if (!block) {
        res.status = 404;
    } else {
        std::cout << "Try\n";
        std::cout << "block type: " << (*block).type_name() << "\n";
        std::cout << "block dump: " << (*block).dump() << "\n";
        std::string content = (*block)["content"];
        std::cout << "Success\n";
        std::string content_type =
            nlohmann::to_string((*block)["Content-Type"]);
        if ((*block)["Content-Type"].is_string()) {
            content_type = (*block)["Content-Type"];
        }
        nlohmann::to_string((*block)["Content-Type"]);
        res.set_content(content, content_type);
    }
}

void createOrModifyBlockHandler(const httplib::Request &req,
                                httplib::Response &res, UDSF &udsf) {
    std::cout << "createOrModifyBlock\n";
    Status status = DEFAULT;
    udsf.createOrModifyBlock(RecordPath(req.path_params.at("realmId"),
                                        req.path_params.at("storageId"),
                                        req.path_params.at("recordId")),
                             req.path_params.at("blockId"), req.body,
                             req.get_header_value("Content-Type"), status);
    if (status == CREATED) {
        res.status = 201;
    } else if (status == MODIFIED) {
        res.status = 204;
    } else if (status == NOT_FOUND) {
        res.status = 404;
    }
}

void deleteBlockHandler(const httplib::Request &req, httplib::Response &res,
                        UDSF &udsf) {
    std::cout << "deleteBlock\n";
    Status status = DEFAULT;
    udsf.deleteBlock(RecordPath(req.path_params.at("realmId"),
                                req.path_params.at("storageId"),
                                req.path_params.at("recordId")),
                     req.path_params.at("blockId"), status);
    if (status == DELETED) {
        res.status = 201;
    } else if (status == NOT_FOUND) {
        res.status = 404;
    }
}

};  // namespace

void UDSFServer::run(std::string host, unsigned int port) {
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
    registerHandlers(server, udsf);
    server.listen(host, port);
}

void UDSFServer::registerHandlers(httplib::Server &server, UDSF &udsf) {
    server.Get("/nudsf-dr/v1/:realmId/:storageId/records/:recordId",
               [&udsf](const httplib::Request &req, httplib::Response &res) {
                   getRecordHandler(req, res, udsf);
               });  // getRecord

    server.Put("/nudsf-dr/v1/:realmId/:storageId/records/:recordId",
               [&udsf](const httplib::Request &req, httplib::Response &res) {
                   createOrModifyRecordHandler(req, res, udsf);
               });  // createOrModifyRecord

    server.Delete("/nudsf-dr/v1/:realmId/:storageId/records/:recordId",
                  [&udsf](const httplib::Request &req, httplib::Response &res) {
                      deleteRecordHandler(req, res, udsf);
                  });  // deleteRecord

    server.Get("/nudsf-dr/v1/:realmId/:storageId/records/:recordId/blocks",
               [&udsf](const httplib::Request &req, httplib::Response &res) {
                   getBlockListHandler(req, res, udsf);
               });  // getBlockList

    server.Get(
        "/nudsf-dr/v1/:realmId/:storageId/records/:recordId/blocks/:blockId",
        [&udsf](const httplib::Request &req, httplib::Response &res) {
            getBlockHandler(req, res, udsf);
        });  // getBlock

    server.Put(
        "/nudsf-dr/v1/:realmId/:storageId/records/:recordId/blocks/:blockId",
        [&udsf](const httplib::Request &req, httplib::Response &res) {
            createOrModifyBlockHandler(req, res, udsf);
        });  // createOrModifyBlock

    server.Delete(
        "/nudsf-dr/v1/:realmId/:storageId/records/:recordId/blocks/:blockId",
        [&udsf](const httplib::Request &req, httplib::Response &res) {
            deleteBlockHandler(req, res, udsf);
        });  // deleteBlock
}
