#pragma once
#include "pch.h"
#include "StorageProvider.h"
#include "AnswerDatabase.h"

class FirebaseDatabaseStorageProvider final : public AnswersStorageProvider
{
public:
    std::shared_ptr<AnswersData> ReadDatabase(const StorageConfig& config) override;
    EFileSaveResult AddNewRecords(const std::shared_ptr<AnswersData>& records, const StorageConfig& config) override;
};
