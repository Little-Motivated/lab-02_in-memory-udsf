#include "udsf.hpp"
#include "httplib.hpp"

#include <optional>

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

std::optional<std::reference_wrapper<Record>>
UDSF::getRecordLink(const RecordPath &path) {
    auto records_map_it = realms_storages.find({path.realmId, path.storageId});
    if (records_map_it == realms_storages.end()) {
        return std::nullopt;
    }
    auto &records_map = records_map_it->second;
    auto record_it = records_map.find(path.recordId);
    if (record_it == records_map.end()) {
        return std::nullopt;
    }
    return std::ref(record_it->second);
}

std::optional<Record> UDSF::getRecord(const RecordPath &path) const {
    auto records_map_it = realms_storages.find({path.realmId, path.storageId});
    if (records_map_it == realms_storages.end()) {
        return std::nullopt;
    }
    auto &records_map = records_map_it->second;
    auto record_it = records_map.find(path.recordId);
    if (record_it == records_map.end()) {
        return std::nullopt;
    }
    return record_it->second;
}

void UDSF::createOrModifyRecord(const RecordPath &path, std::string body,
                                Status &status) {
    // get-previous? read blocks from body?
    status = MODIFIED;
    auto &records_map = realms_storages[{path.realmId, path.storageId}];
    auto &record = records_map[path.recordId];
    if (record.recordId == "") {
        status = CREATED;
    }
    std::cout << "Parse\n";
    nlohmann::json j = nlohmann::json::parse(body);  // body = "" -> exception
    std::cout << "Write meta\n";
    record.recordId = path.recordId;
    record.meta = j["meta"];
    if (j.contains("blocks")) {
        std::cout << "Write blocks\n";
        for (auto block : j["blocks"]) {
            record.blocks[block["Content-Id"]] = block;  // check block,
                                                         // content-type...
        }
        std::cout << "Done\n";
    }
}

void UDSF::deleteRecord(const RecordPath &path, Status &status) {
    // get-previous
    auto records_map_it = realms_storages.find({path.realmId, path.storageId});
    if (records_map_it == realms_storages.end()) {
        status = NOT_FOUND;
        return;
    }
    auto &records_map = records_map_it->second;
    auto record_it = records_map.find(path.recordId);
    if (record_it == records_map.end()) {
        status = NOT_FOUND;
        return;
    }
    records_map.erase(record_it);
    status = DELETED;
}

std::optional<Record> UDSF::getBlockList(const RecordPath &path) const {
    auto record = getRecord(path);
    if (!record || record->blocks.size() == 0) {
        return std::nullopt;
    }
    return record;
}

std::optional<nlohmann::json> UDSF::getBlock(const RecordPath &path,
                                             std::string blockId) const {
    auto record = getBlockList(path);
    if (!record) {
        return std::nullopt;
    }
    auto block_it = record->blocks.find(blockId);
    if (block_it == record->blocks.end()) {
        return std::nullopt;
    }
    return block_it->second;
}

void UDSF::createOrModifyBlock(
    const RecordPath &path, std::string blockId, std::string body,
    std::string content_type,
    Status &status) {  // header instead of content-type
    status = MODIFIED;
    auto records_map_it = realms_storages.find({path.realmId, path.storageId});
    if (records_map_it == realms_storages.end()) {
        status = NOT_FOUND;
        return;
    }
    auto &records_map = records_map_it->second;
    auto record_it = records_map.find(path.recordId);
    if (record_it == records_map.end()) {
        status = NOT_FOUND;
        return;
    }
    if (record_it->second.blocks.count(blockId) == 0 ||
        record_it->second.blocks[blockId] == "") {
        status = CREATED;
    }
    nlohmann::json j;  // = nlohmann::json::parse(body);
    j["Content-Id"] = blockId;
    j["Content-Type"] = content_type;
    j["content"] = body;  // Content? no parse?
    record_it->second.blocks[blockId] = j;
}

void UDSF::deleteBlock(const RecordPath &path, std::string blockId,
                       Status &status) {
    auto records_map_it = realms_storages.find({path.realmId, path.storageId});
    if (records_map_it == realms_storages.end()) {
        status = NOT_FOUND;
        return;
    }
    auto &records_map = records_map_it->second;
    auto record_it = records_map.find(path.recordId);
    if (record_it == records_map.end()) {
        status = NOT_FOUND;
        return;
    }
    if (record_it->second.blocks.count(blockId) == 0) {
        status = NOT_FOUND;
        return;
    }
    record_it->second.blocks.erase(blockId);
    status = DELETED;
}
