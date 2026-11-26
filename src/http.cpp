#include "httplib.hpp"
#include "nlohmann/json.hpp"
#include "udsf.hpp"

#include <optional>

void registerHandlers(httplib::Server &server, UDSF &udsf) {
    server.Get(
        "/nudsf-dr/v1/:realmId/:storageId/records/:recordId",
        [&udsf](const httplib::Request &req, httplib::Response &res) {
            std::cout << "getRecord\n";
            std::optional<Record> record = udsf.getRecord(RecordPath(
                req.path_params.at("realmId"), req.path_params.at("storageId"),
                req.path_params.at("recordId")));
            if (record == std::nullopt) {
                res.status = 404;
            } else {
                res.status = 200;
                res.set_content((*record).toJsonString(), "application/json");
            }
        });  // getRecord

    server.Put("/nudsf-dr/v1/:realmId/:storageId/records/:recordId",
               [&udsf](const httplib::Request &req, httplib::Response &res) {
                   std::cout << "createOrModifyRecord\n";
                   Status status = DEFAULT;
                   udsf.createOrModifyRecord(
                       RecordPath(req.path_params.at("realmId"),
                                  req.path_params.at("storageId"),
                                  req.path_params.at("recordId")),
                       req.body, status);
                   if (status == CREATED) {
                       res.status = 201;
                   } else if (status == FOUND) {
                       res.status = 204;
                   }
               });  // createOrModifyRecord

    server.Delete("/nudsf-dr/v1/:realmId/:storageId/records/:recordId",
                  [&udsf](const httplib::Request &req, httplib::Response &res) {
                      std::cout << "deleteRecord\n";
                      Status status = DEFAULT;
                      udsf.deleteRecord(
                          RecordPath(req.path_params.at("realmId"),
                                     req.path_params.at("storageId"),
                                     req.path_params.at("recordId")),
                          status);
                      if (status == NOT_FOUND) {
                          res.status = 404;
                      } else if (status == DELETED) {
                          res.status = 204;
                      }
                  });  // deleteRecord

    server.Get("/nudsf-dr/v1/:realmId/:storageId/records/:recordId/blocks",
               [&udsf](const httplib::Request &req, httplib::Response &res) {
                   std::cout << "getBlockList\n";
                   std::optional<Record> record = udsf.getBlockList(
                       RecordPath(req.path_params.at("realmId"),
                                  req.path_params.at("storageId"),
                                  req.path_params.at("recordId")));
                   if (record == std::nullopt) {
                       res.status = 404;
                   } else {
                       res.status = 200;
                       res.set_content((*record).toJsonStringNoMeta(),
                                       "application/json");
                   }
               });  // getBlockList

    server.Get(
        "/nudsf-dr/v1/:realmId/:storageId/records/:recordId/blocks/:blockId",
        [&udsf](const httplib::Request &req, httplib::Response &res) {
            std::cout << "getBlock\n";
            auto block =
                udsf.getBlock(RecordPath(req.path_params.at("realmId"),
                                         req.path_params.at("storageId"),
                                         req.path_params.at("recordId")),
                              req.path_params.at("blockId"));
            if (!block) {
                res.status = 404;
            } else {
                std::string content = (*block)["content"];
                std::string content_type =
                    nlohmann::to_string((*block)["Content-Type"]);
                if ((*block)["Content-Type"].is_string()) {
                    content_type = (*block)["Content-Type"];
                }
                nlohmann::to_string((*block)["Content-Type"]);
                res.set_content(content, content_type);
            }
        });  // getBlock

    server.Put(
        "/nudsf-dr/v1/:realmId/:storageId/records/:recordId/blocks/:blockId",
        [&udsf](const httplib::Request &req, httplib::Response &res) {
            std::cout << "createOrModifyBlock\n";
            Status status = DEFAULT;
            udsf.createOrModifyBlock(RecordPath(req.path_params.at("realmId"),
                                                req.path_params.at("storageId"),
                                                req.path_params.at("recordId")),
                                     req.path_params.at("blockId"), req.body,
                                     status);
            if (status == FOUND) {
                res.status = 201;
            } else if (status == CREATED) {
                res.status = 204;
            } else if (status == NOT_FOUND) {
                res.status = 404;
            }
        });  // createOrModifyBlock

    server.Delete(
        "/nudsf-dr/v1/:realmId/:storageId/records/:recordId/blocks/:blockId",
        [&udsf](const httplib::Request &req, httplib::Response &res) {
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
        });  // deleteBlock
}
