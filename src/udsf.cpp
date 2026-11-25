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
    j["blocks"] = blocks;
    j["meta"] = meta;
    return nlohmann::to_string(j);
}

std::string Record::toJsonStringNoMeta() const {
    nlohmann::json j;
    j["blocks"] = blocks;
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
    nlohmann::json j = nlohmann::json::parse(body);  // body = "" -> exception

    Record new_record(*record_it);
    new_record.meta = nlohmann::to_string(j["meta"]);
    records_set.erase(record_it);
    records_set.insert(new_record);
}

void UDSF::deleteRecord(const RecordPath &path,
                        Status &status) {  // get-previous
    if (realms_storages.count({path.realmId, path.storageId}) == 0) {
        status = NOT_FOUND;
        return;
    }
    auto records_set = realms_storages[{path.realmId, path.storageId}];
    auto record = records_set.find(path.recordId);
    if (record == records_set.end()) {
        status = NOT_FOUND;
        return;
    }
    records_set.erase(record);
    status = DELETED;
}

void UDSF::getBlockList(const RecordPath &path) const {
}

void UDSF::getBlock(const RecordPath &path, std::string blockId) {
}

void UDSF::createOrModifyBlock(const RecordPath &path, std::string blockId,
                               std::string body, Status &status) {
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
    if (new_record.blocks.count(blockId) == 0) {
        status = CREATED;
    }
    new_record.blocks[blockId] = body;
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
