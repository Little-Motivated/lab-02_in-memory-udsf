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

std::optional<Record> UDSF::getRecord(const RecordPath &path) const {
    std::cout << "create\n";
    std::cout << path.realmId << "\n";
    std::cout << path.storageId << "\n";
    std::cout << path.recordId << "\n";
    std::cout << "Storage size: " << realms_storages.size() << "\n";
    auto records_set_it = realms_storages.find(
        std::pair<std::string, std::string>(path.realmId, path.storageId));
    if (records_set_it == realms_storages.end()) {
        std::cout << "Cant find records_set\n\n";
        return std::nullopt;
    }
    auto records_set = (*records_set_it).second;
    std::cout << "Set size: " << records_set.size() << "\n\n";

    auto record = records_set.find(path.recordId);
    if (record == records_set.end()) {
        std::cout << "Cant find record\n\n";
        return std::nullopt;
    }
    return *record;
}

void UDSF::createOrModifyRecord(
    const RecordPath &path, std::string body,
    Status &status) {  // get-previous? read blocks from body?
    auto &records_set = realms_storages[{path.realmId, path.storageId}];
    auto record_it = records_set.find(path.recordId);
    status = FOUND;
    if (record_it == records_set.end()) {
        records_set.insert(Record(path.recordId));
        record_it = records_set.find(path.recordId);
        status = CREATED;
    }
    nlohmann::json j = nlohmann::json::parse(body);

    Record new_record(*record_it);
    new_record.meta = nlohmann::to_string(j["meta"]);
    records_set.erase(record_it);
    records_set.insert(new_record);
    std::cout << "create\n";
    std::cout << path.realmId << "\n";
    std::cout << path.storageId << "\n";
    std::cout << path.recordId << "\n";
    std::cout << realms_storages.size() << "\n\n";
}

void UDSF::deleteRecord(const RecordPath &path,
                        Status &status) {  // get-previous
    auto records_set_it = realms_storages.find(
        std::pair<std::string, std::string>(path.realmId, path.storageId));
    if (records_set_it == realms_storages.end()) {
        status = NOT_FOUND;
        return;
    }
    auto &records_set = (*records_set_it).second;
    auto record = records_set.find(path.recordId);
    if (record == records_set.end()) {
        status = NOT_FOUND;
        return;
    }
    records_set.erase(record);
    status = DELETED;
    return;
}

void UDSF::getBlockList(const RecordPath &path) const {
}

void UDSF::getBlock(const RecordPath &path, std::string blockId) const {
}

void UDSF::createOrModifyBlock(const RecordPath &path, std::string blockId) {
}

void UDSF::deleteBlock(const RecordPath &path, std::string blockId) {
}
