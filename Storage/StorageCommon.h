#pragma once
#include "pch.h"
#include "cute_enums.h"

enum class ELocation : uint8
{
    ApplicationFolder,
    WindowsTempFolder,
    CustomFolder
};

enum class EStorageBackend : uint8
{
    BinaryFile,
    JsonFile,
    FirebaseDatabase
};

struct StorageConfig
{
    ELocation location{ ELocation::ApplicationFolder };
    std::filesystem::path custom_folder_path;
};
