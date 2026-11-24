#pragma once
#include "httplib.h"

class UDSF {
public:
    void getRecord() const;
    void createOrModifyRecord();
    void deleteRecord();
    void getBlockList() const;
    void getBlock() const;
    void createOrModifyBlock();
    void deleteBlock();

    void save();
    void load();
};