#include "httplib.h"
#include "udsf.h"

void registerHandlers(httplib::Server &server, UDSF &udsf) {
    server.Get("/nudsf/v1/:realmId/:storageId/records/:recordId",
               [](const httplib::Request &req, httplib::Response &res) {
                   std::cout << "getRecord\n";
                   std::cout << req.path_params.at("realmId") << "\n";
                   std::cout << req.path_params.at("storageId") << "\n";
                   std::cout << req.path_params.at("recordId") << "\n";
               });  // getRecord

    server.Put("/nudsf/v1/:realmId/:storageId/records/:recordId",
               [](const httplib::Request &req, httplib::Response &res) {
                   std::cout << "createOrModifyRecord\n";
                   std::cout << req.path_params.at("realmId") << "\n";
                   std::cout << req.path_params.at("storageId") << "\n";
                   std::cout << req.path_params.at("recordId") << "\n";
               });  // createOrModifyRecord

    server.Delete("/nudsf/v1/:realmId/:storageId/records/:recordId",
                  [](const httplib::Request &req, httplib::Response &res) {
                      std::cout << "deleteRecord\n";
                      std::cout << req.path_params.at("realmId") << "\n";
                      std::cout << req.path_params.at("storageId") << "\n";
                      std::cout << req.path_params.at("recordId") << "\n";
                  });  // deleteRecord

    server.Get("/nudsf/v1/:realmId/:storageId/records/:recordId",
               [](const httplib::Request &req, httplib::Response &res) {
                   std::cout << "getBlockList\n";
                   std::cout << req.path_params.at("realmId") << "\n";
                   std::cout << req.path_params.at("storageId") << "\n";
                   std::cout << req.path_params.at("recordId") << "\n";
               });  // getBlockList

    server.Put("/nudsf/v1/:realmId/:storageId/records/:recordId",
               [](const httplib::Request &req, httplib::Response &res) {
                   std::cout << "createOrModifyBlock\n";
                   std::cout << req.path_params.at("realmId") << "\n";
                   std::cout << req.path_params.at("storageId") << "\n";
                   std::cout << req.path_params.at("recordId") << "\n";
               });  // createOrModifyBlock

    server.Delete("/nudsf/v1/:realmId/:storageId/records/:recordId",
                  [](const httplib::Request &req, httplib::Response &res) {
                      std::cout << "deleteBlock\n";
                      std::cout << req.path_params.at("realmId") << "\n";
                      std::cout << req.path_params.at("storageId") << "\n";
                      std::cout << req.path_params.at("recordId") << "\n";
                  });  // deleteBlock
}
