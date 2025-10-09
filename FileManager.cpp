#include "pch.h"
#include "FileManager.h"
#include "AnswerDatabase.h"
#include "Storage/StorageProvider.h"
#include "Storage/BinaryFileStorageProvider.h"
#include "Storage/JsonFileStorageProvider.h"
#include "Storage/FirebaseDatabaseStorageProvider.h"

namespace
{
    size_t g_excercises_saved = 0;

#if defined(USE_JSON_STORAGE)
    constexpr EStorageBackend DEFAULT_BACKEND = EStorageBackend::JsonFile;
#elif defined(USE_FIREBASE_STORAGE)
    constexpr EStorageBackend DEFAULT_BACKEND = EStorageBackend::FirebaseDatabase;
#else
    constexpr EStorageBackend DEFAULT_BACKEND = EStorageBackend::BinaryFile;
#endif

    EStorageBackend g_current_backend = DEFAULT_BACKEND;
    StorageConfig g_storage_config{};
    std::unique_ptr<AnswersStorageProvider> g_provider;

    std::unique_ptr<AnswersStorageProvider> CreateProvider(EStorageBackend backend)
    {
        switch (backend)
        {
        case EStorageBackend::BinaryFile:
            return std::make_unique<BinaryFileStorageProvider>();
        case EStorageBackend::JsonFile:
            return std::make_unique<JsonFileStorageProvider>();
        case EStorageBackend::FirebaseDatabase:
            return std::make_unique<FirebaseDatabaseStorageProvider>();
        default:
            return {};
        }
    }

    AnswersStorageProvider* GetOrCreateProvider()
    {
        if (!g_provider)
        {
            g_provider = CreateProvider(g_current_backend);
        }
        return g_provider.get();
    }
}

auto FileManager::ReadDatabase() -> std::shared_ptr<AnswersData>
{
    StorageConfig config = g_storage_config;
    config.location = ELocation::ApplicationFolder;
    auto* const provider = GetOrCreateProvider();
    if (!provider)
    {
        return std::shared_ptr<AnswersData>(nullptr);
    }
    return provider->ReadDatabase(config);
}

EFileSaveResult FileManager::AddNewRecords(
    std::shared_ptr<AnswersData> new_records, 
    ELocation directory)
{
    StorageConfig config = g_storage_config;
    config.location = directory;
    auto* const provider = GetOrCreateProvider();
    if (!provider)
    {
        return EFileSaveResult::CannotCreateFile;
    }
    const auto result = provider->AddNewRecords(new_records, config);
    if (result == EFileSaveResult::Success && directory == ELocation::ApplicationFolder)
    {
        g_excercises_saved += new_records->GetExcerciseCount();
    }
    return result;
}

size_t FileManager::GetSavedExcercisesCount() noexcept
{
    return g_excercises_saved;
}

void FileManager::SetStorageBackend(EStorageBackend backend) noexcept
{
    if (g_current_backend == backend)
    {
        return;
    }
    g_current_backend = backend;
    g_provider.reset();
}

void FileManager::SetStorageConfig(const StorageConfig& config)
{
    g_storage_config = config;
}

EStorageBackend FileManager::GetStorageBackend() noexcept
{
    return g_current_backend;
}

StorageConfig FileManager::GetStorageConfig()
{
    return g_storage_config;
}
