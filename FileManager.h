#pragma once
#include "pch.h"
#include "Storage/StorageCommon.h"

class AnswersData;

struct FileManager
{
    static void SetStorageBackend(EStorageBackend backend) noexcept;
    static void SetStorageConfig(const StorageConfig& config);
    [[nodiscard]] static EStorageBackend GetStorageBackend() noexcept;
    [[nodiscard]] static StorageConfig GetStorageConfig();

    static [[nodiscard]] size_t GetSavedExcercisesCount() noexcept;
    static [[nodiscard]] auto ReadDatabase() -> std::shared_ptr<AnswersData>;
    static [[nodiscard]] EFileSaveResult AddNewRecords(std::shared_ptr<AnswersData> records, ELocation directory = ELocation::ApplicationFolder);

private:
    FileManager() = delete;
    ~FileManager() = delete;
};
