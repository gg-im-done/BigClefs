#include "pch.h"
#include "FirebaseDatabaseStorageProvider.h"
#include "AnswerDatabase.h"

std::shared_ptr<AnswersData> FirebaseDatabaseStorageProvider::ReadDatabase(const StorageConfig&)
{
    return std::make_shared<AnswersData>();
}

EFileSaveResult FirebaseDatabaseStorageProvider::AddNewRecords(const std::shared_ptr<AnswersData>& records, const StorageConfig&)
{
    if (!records || records->GetExcerciseCount() == 0)
    {
        return EFileSaveResult::NothingToSave;
    }
    return EFileSaveResult::Success;
}
