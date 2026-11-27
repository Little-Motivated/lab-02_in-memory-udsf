#include "udsf.hpp"
#include "httplib.hpp"

#include <optional>

bool Record::operator<(std::string _recordId) const noexcept {
    return recordId < _recordId;
}

bool Record::operator()(const Record record) const noexcept {
    return recordId < record.recordId;
}

std::string Record::toJsonString() const {
    nlohmann::json j;
    if (blocks.size() > 0) {
        std::cout << "Blocks added to asnwer\n";
        for (auto block : blocks) {
            j["blocks"].push_back(block.second);
        }
    }
    j["meta"] = meta;
    return nlohmann::to_string(j);
}

std::string Record::toJsonStringMeta() const {  // not need
    nlohmann::json j;
    j["meta"] = meta;
    return nlohmann::to_string(j);
}

std::string Record::toJsonStringNoMeta() const {
    nlohmann::json j;
    if (blocks.size() > 0) {
        std::cout << "Blocks added to asnwer\n";
        for (auto block : blocks) {
            j.push_back(block.second);
        }
    }
    return nlohmann::to_string(j);
}

bool operator<(const Record &lhs, const Record &rhs) noexcept {
    return lhs.recordId < rhs.recordId;
}

std::optional<Record>
UDSF::getRecord(const RecordPath &path) {  // find -> const method
    if (realms_storages.count({path.realmId, path.storageId}) == 0) {
        return std::nullopt;
    }
    auto records_set = realms_storages[{path.realmId, path.storageId}];
    auto record = records_set.find(path.recordId);
    if (record == records_set.end()) {
        return std::nullopt;
    }
    return *record;
}

void UDSF::createOrModifyRecord(
    const RecordPath &path, std::string body,
    Status &status) {  // get-previous? read blocks from body?
    auto &records_set = realms_storages[{path.realmId, path.storageId}];
    status = FOUND;
    auto record_it = records_set.find(path.recordId);
    if (record_it == records_set.end()) {
        records_set.insert(Record(path.recordId));
        record_it = records_set.find(path.recordId);
        status = CREATED;
    }
    std::cout << "Parse\n";
    nlohmann::json j = nlohmann::json::parse(body);  // body = "" -> exception

    Record new_record(*record_it);
    std::cout << "Write meta\n";
    new_record.meta = j["meta"];
    if (j.contains("blocks")) {
        std::cout << "Write blocks\n";
        for (auto block : j["blocks"]) {
            new_record.blocks[block["Content-Id"]] = block;  // check block,
                                                             // content-type...
        }
        std::cout << "Done\n";
    }
    records_set.erase(record_it);
    records_set.insert(new_record);
}

void UDSF::deleteRecord(const RecordPath &path,
                        Status &status) {  // get-previous
    if (realms_storages.count({path.realmId, path.storageId}) == 0) {
        status = NOT_FOUND;
        return;
    }
    auto &records_set = realms_storages[{path.realmId, path.storageId}];
    auto record = records_set.find(path.recordId);
    if (record == records_set.end()) {
        status = NOT_FOUND;
        return;
    }
    records_set.erase(record);
    status = DELETED;
}

std::optional<Record> UDSF::getBlockList(const RecordPath &path) {
    auto record = getRecord(path);
    if (!record || (*record).blocks.size() == 0) {
        return std::nullopt;
    }
    return record;
}

std::optional<nlohmann::json> UDSF::getBlock(const RecordPath &path,
                                             std::string blockId) {
    auto record = getBlockList(path);
    if (!record) {
        return std::nullopt;
    }

    nlohmann::json j((*record).blocks);
    if (j.contains(blockId)) {
        std::cout << "dgsfgbda\n\n";
        std::cout << (j[blockId]) << "\n";
        // std::string block = j[blockId];
        // return nlohmann::json::parse(block);
        return j[blockId];
    }
    return std::nullopt;
}

void UDSF::createOrModifyBlock(const RecordPath &path, std::string blockId,
                               std::string body, std::string content_type,
                               Status &status) {  // content-type
    if (realms_storages.count({path.realmId, path.storageId}) == 0) {
        status = NOT_FOUND;
        return;
    }
    auto &records_set = realms_storages[{path.realmId, path.storageId}];
    auto record_it = records_set.find(path.recordId);
    status = FOUND;
    if (record_it == records_set.end()) {
        status = NOT_FOUND;
        return;
    }
    Record new_record(*record_it);
    if (new_record.blocks.count(blockId) == 0 ||
        new_record.blocks[blockId] == "") {
        status = CREATED;
    }
    nlohmann::json j;  // = nlohmann::json::parse(body);
    j["Content-Id"] = blockId;
    j["Content-Type"] = content_type;
    j["content"] = body;  // Content? no parse?
    new_record.blocks[blockId] = j;
    records_set.erase(record_it);
    records_set.insert(new_record);
}

void UDSF::deleteBlock(const RecordPath &path, std::string blockId,
                       Status &status) {
    if (realms_storages.count({path.realmId, path.storageId}) == 0) {
        status = NOT_FOUND;
        return;
    }
    auto &records_set = realms_storages[{path.realmId, path.storageId}];
    auto record_it = records_set.find(path.recordId);
    status = DELETED;
    if (record_it == records_set.end()) {
        status = NOT_FOUND;
        return;
    }
    Record new_record(*record_it);
    if (new_record.blocks.count(blockId) == 0) {
        status = NOT_FOUND;
    }
    new_record.blocks.erase(blockId);
    records_set.erase(record_it);
    records_set.insert(new_record);
}
