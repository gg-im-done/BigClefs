#pragma once
#include "pch.h"
#include "StorageCommon.h"

class AnswersData;

class AnswersStorageProvider
{
public:
    virtual ~AnswersStorageProvider() = default;
    virtual std::shared_ptr<AnswersData> ReadDatabase(const StorageConfig& config) = 0;
    virtual EFileSaveResult AddNewRecords(const std::shared_ptr<AnswersData>& records, const StorageConfig& config) = 0;
};
