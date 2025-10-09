#include "pch.h"
#include "JsonFileStorageProvider.h"
#include "AnswerDatabase.h"
#include "ExerciseResult.h"
#include "Answer.h"
#include "Note.h"
#include <nlohmann/json.hpp>
#include <system_error>

namespace
{
    std::filesystem::path GetFolderPathForLocation(const StorageConfig& config)
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

    std::filesystem::path GetDatabaseFilePath(const StorageConfig& config)
    {
        const auto folder_path = GetFolderPathForLocation(config);
        return folder_path / L"answers.json";
    }

    nlohmann::json SerializeAnswer(const Answer& answer)
    {
        nlohmann::json j;
        j["time_ms"] = static_cast<std::int64_t>(answer.GetTime().count());
        j["specific_note"] = static_cast<int>(answer.GetNote().GetSpecificNote());
        j["clef"] = static_cast<int>(answer.GetNote().GetClef());
        j["is_correct"] = answer.IsCorrect();
        return j;
    }

    Answer DeserializeAnswer(const nlohmann::json& j)
    {
        const auto time_ms = std::chrono::milliseconds(j.value("time_ms", 0));
        const auto specific_note = static_cast<ESpecificNote>(j.value("specific_note", static_cast<int>(ESpecificNote::NOTE_A_0)));
        const auto clef = static_cast<EClefType>(j.value("clef", static_cast<int>(EClefType::Treble)));
        const bool is_correct = j.value("is_correct", false);
        const Note note(specific_note, clef);
        return Answer(note, time_ms, is_correct);
    }
}

std::shared_ptr<AnswersData> JsonFileStorageProvider::ReadDatabase(const StorageConfig& config)
{
    const auto database_file_path = GetDatabaseFilePath(config);
    std::ifstream file_in(database_file_path);
    if (!file_in.is_open())
    {
        return std::shared_ptr<AnswersData>(nullptr);
    }

    nlohmann::json root;
    try
    {
        file_in >> root;
    }
    catch (...)
    {
        return std::make_shared<AnswersData>();
    }

    auto out_database = std::make_shared<AnswersData>();
    try
    {
        const auto& exercises = root.contains("exercises") ? root["exercises"] : nlohmann::json::array();
        for (const auto& exercise : exercises)
        {
            const auto date_time = static_cast<time_t>(exercise.value("date_time", static_cast<std::int64_t>(0)));
            const auto& answers_json = exercise.contains("answers") ? exercise["answers"] : nlohmann::json::array();
            std::vector<Answer> answers;
            answers.reserve(answers_json.size());
            for (const auto& a : answers_json)
            {
                answers.emplace_back(DeserializeAnswer(a));
            }
            ExerciseResult exercise_result(answers, date_time);
            out_database->AddExcerciseResult(std::move(exercise_result));
        }
    }
    catch (...)
    {
        return std::make_shared<AnswersData>();
    }

    return out_database;
}

EFileSaveResult JsonFileStorageProvider::AddNewRecords(const std::shared_ptr<AnswersData>& records, const StorageConfig& config)
{
    if (!records || records->GetExcerciseCount() == 0)
    {
        return EFileSaveResult::NothingToSave;
    }

    nlohmann::json root;

    const auto database_file_path = GetDatabaseFilePath(config);

    {
        std::ifstream existing_in(database_file_path);
        if (existing_in.is_open())
        {
            try
            {
                existing_in >> root;
            }
            catch (...)
            {
                root = nlohmann::json::object();
            }
        }
    }

    if (!root.is_object())
    {
        root = nlohmann::json::object();
    }

    if (!root.contains("exercises") || !root["exercises"].is_array())
    {
        root["exercises"] = nlohmann::json::array();
    }

    const auto& exercise_results = records->GetArrayRef();
    for (const auto& exercise_result : exercise_results)
    {
        nlohmann::json exercise_json;
        exercise_json["date_time"] = static_cast<std::int64_t>(exercise_result.GetDateTime());
        nlohmann::json answers_json = nlohmann::json::array();
        const auto& answers = exercise_result.GetAnswersArrayRef();
        for (const auto& answer : answers)
        {
            answers_json.push_back(SerializeAnswer(answer));
        }
        exercise_json["answers"] = std::move(answers_json);
        root["exercises"].push_back(std::move(exercise_json));
    }

    const auto folder_path = GetFolderPathForLocation(config);
    if (!folder_path.empty())
    {
        std::error_code error;
        std::filesystem::create_directories(folder_path, error);
    }

    std::ofstream file_out(database_file_path, std::ios::out | std::ios::trunc);
    if (!file_out.is_open())
    {
        return EFileSaveResult::CannotCreateFile;
    }
    const std::string serialized = root.dump(2);
    file_out << serialized;

    return EFileSaveResult::Success;
}
