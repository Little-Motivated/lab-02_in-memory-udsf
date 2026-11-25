#pragma once
#include "httplib.hpp"
#include "nlohmann/json.hpp"

#include <optional>

enum Status { DEFAULT, NOT_FOUND, FOUND, CREATED, MODIFIED, DELETED };

class Record {
private:
    std::string recordId;

public:
    std::string meta;
    std::map<std::string, std::string> blocks;
    bool operator<(std::string _recordId) const noexcept;
    bool operator()(const Record) const noexcept;

    Record(std::string _recordId) : recordId(_recordId) {
    }

    std::string toJsonString() const;
    std::string toJsonStringNoMeta() const;

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
    std::map<std::pair<std::string, std::string>, std::set<Record>>
        realms_storages;
    // void findRecord(RecordPath &path) const;

public:
    std::optional<Record> getRecord(const RecordPath &path);
    void createOrModifyRecord(const RecordPath &path, std::string body,
                              Status &status);
    void deleteRecord(const RecordPath &path, Status &status);
    void getBlockList(const RecordPath &path) const;
    void getBlock(const RecordPath &path, std::string blockId);
    void createOrModifyBlock(const RecordPath &path, std::string blockId,
                             std::string body, Status &status);
    void deleteBlock(const RecordPath &path, std::string blockId,
                     Status &status);

    void save();
    void load();
};