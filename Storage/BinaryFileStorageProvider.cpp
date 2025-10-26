#include "pch.h"
#include "BinaryFileStorageProvider.h"
#include "AnswerDatabase.h"
#include "ExerciseResult.h"

namespace
{
    static std::filesystem::path GetFolderPathForLocation(const StorageConfig& config)
    {
        switch (config.location)
        {
        case ELocation::ApplicationFolder:
        {
            const auto executable_wpath = wxStandardPaths::Get().GetExecutablePath().ToStdWstring();
            const std::filesystem::path executable_path(executable_wpath);
            return executable_path.parent_path();
        }
        case ELocation::WindowsTempFolder:
        {
            const auto temp_dir_wpath = wxStandardPaths::Get().GetTempDir().ToStdWstring();
            return std::filesystem::path(temp_dir_wpath);
        }
        case ELocation::CustomFolder:
        {
            return config.custom_folder_path;
        }
        default:
            return {};
        }
    }

    static std::filesystem::path GetDatabaseFilePath(const StorageConfig& config)
    {
        const auto folder_path = GetFolderPathForLocation(config);
        return folder_path / L"answers.bin";
    }
}

std::shared_ptr<AnswersData> BinaryFileStorageProvider::ReadDatabase(const StorageConfig& config)
{
    const auto database_file_path = GetDatabaseFilePath(config);

    std::ifstream file_in(database_file_path, std::ios::in | std::ios::binary);
    if (!file_in.is_open())
    {
        return std::shared_ptr<AnswersData>(nullptr);
    }

    auto out_database = std::make_shared<AnswersData>();
    for (;;)
    {
        time_t date_time{};
        if (!file_in.read(reinterpret_cast<char*>(&date_time), sizeof(date_time)))
        {
            break;
        }

        size_t answers_size{};
        if (!file_in.read(reinterpret_cast<char*>(&answers_size), sizeof(answers_size)))
        {
            break;
        }

        if (answers_size == 0)
        {
            throw std::runtime_error("Reading 0 answers? Data file corruption?");
        }
        if (answers_size > 6969)
        {
            throw std::runtime_error("Reading to many answers. WTF happened?");
        }

        std::vector<Answer> deserialized_answers;
        deserialized_answers.resize(answers_size);
        const auto bytes_to_read = answers_size * sizeof(Answer);
        if (!file_in.read(reinterpret_cast<char*>(deserialized_answers.data()), static_cast<std::streamsize>(bytes_to_read)))
        {
            break;
        }

        ExerciseResult exercise_result(deserialized_answers, date_time);
        out_database->AddExerciseResult(std::move(exercise_result));
    }
    return out_database;
}

EFileSaveResult BinaryFileStorageProvider::AddNewRecords(const std::shared_ptr<AnswersData>& records, const StorageConfig& config)
{
    if (records->GetExerciseCount() == 0)
    {
        return EFileSaveResult::NothingToSave;
    }

    const auto database_file_path = GetDatabaseFilePath(config);

    std::ofstream file_out(database_file_path, std::ios::out | std::ios::app | std::ios::binary);
    if (!file_out.is_open())
    {
        return EFileSaveResult::CannotCreateFile;
    }

    decltype(auto) v_new_records = records->GetArrayRef();
    for (const ExerciseResult& exercise_result : v_new_records)
    {
        const time_t exercise_date = exercise_result.GetDateTime();
        file_out.write(reinterpret_cast<const char*>(&exercise_date), sizeof(exercise_date));

        decltype(auto) answers = exercise_result.GetAnswersArrayRef();
        if (answers.empty())
        {
            throw std::logic_error("Attempting to write 0 answers?");
        }
        const size_t answers_size = answers.size();
        file_out.write(reinterpret_cast<const char*>(&answers_size), sizeof(answers_size));

        for (const Answer& answer : answers)
        {
            file_out.write(reinterpret_cast<const char*>(&answer), sizeof(answer));
        }
    }

    return EFileSaveResult::Success;
}
