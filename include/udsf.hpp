#pragma once
#include "nlohmann/json.hpp"

#include <optional>

enum Status { DEFAULT, NOT_FOUND, CREATED, MODIFIED, DELETED };

class Record {
public:
    std::string recordId;
    nlohmann::json meta;
    std::map<std::string, nlohmann::json> blocks;

    Record() = default;

    Record(std::string _recordId) : recordId(_recordId) {
    }

    std::string toJsonString() const;
    std::string toJsonStringNoMeta() const;

    bool operator()(const Record) const noexcept;
    friend bool operator<(const Record &lhs, const Record &rhs) noexcept;
};

class RecordPath {
public:
    std::string realmId;
    std::string storageId;
    std::string recordId;

    RecordPath(std::string _realmId, std::string _storageId,
               std::string _recordId)
        : realmId(_realmId), storageId(_storageId), recordId(_recordId) {
    }
};

class UDSF {
private:
    std::map<std::pair<std::string, std::string>, std::map<std::string, Record>>
        realms_storages;
    // void findRecord(RecordPath &path) const;

    std::optional<std::reference_wrapper<Record>>
    getRecordLink(const RecordPath &path);

public:
    std::optional<Record> getRecord(const RecordPath &path) const;
    void createOrModifyRecord(const RecordPath &path, std::string body,
                              Status &status);
    void deleteRecord(const RecordPath &path, Status &status);
    std::optional<Record> getBlockList(const RecordPath &path) const;
    std::optional<nlohmann::json> getBlock(const RecordPath &path,
                                           std::string blockId) const;
    void createOrModifyBlock(const RecordPath &path, std::string blockId,
                             std::string body, std::string content_type,
                             Status &status);
    void deleteBlock(const RecordPath &path, std::string blockId,
                     Status &status);

    void save();
    void load();
};
