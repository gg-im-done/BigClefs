#include "pch.h"
#include "AnswerManager.h"
#include "FileManager.h"

static std::chrono::system_clock::time_point question_start_time;

AnswerManager::AnswerManager() noexcept
{
	answers.reserve(69);
	ResetAllCurrentSessionAnswers();
}

int AnswerManager::GetRatio() const noexcept
{
	return (right_count + wrong_count == 0) ? 0 : int((float(right_count) / float(right_count + wrong_count)) * 100.f);
}

void AnswerManager::ResetAllCurrentSessionAnswers()
{
	pAnswerDatabase = std::make_shared<AnswersData>();
}

void AnswerManager::ClearSession() noexcept
{
	answers.clear();
	right_count = 0;
	wrong_count = 0;
}

void AnswerManager::InitExcerciseSession() noexcept
{
	ClearSession();
	question_start_time = std::chrono::system_clock::now();
}

void AnswerManager::ShutdownExcerciseSession()
{
	if (answers.empty())
	{
		throw std::logic_error("Trying to add an empty excercise result");
	}
	ExerciseResult result(answers);
	pAnswerDatabase->AddExcerciseResult(result);
}

void AnswerManager::AddNewAnswer(std::vector<Note>::const_iterator note_iter, bool is_correct) noexcept
{
	const auto time_now = std::chrono::system_clock::now();
	const auto speed = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - question_start_time);
	answers.emplace_back(*note_iter, speed, is_correct);
	is_correct ? ++right_count : ++wrong_count;
	question_start_time = std::chrono::system_clock::now();
}

std::chrono::milliseconds AnswerManager::GetAverageAnswerTime() const noexcept
{
	if (answers.empty())
		return std::chrono::milliseconds(-1);

	size_t average = 0;
	for (const auto& answer : answers)
	{
		average += answer.GetTime().count();
	}
	average /= answers.size();
	return std::chrono::milliseconds(average);
}

std::string AnswerManager::GetAnswerStatusText() const noexcept
{
	if (answers.empty())
	{
		return std::string();
	}
	return std::format("Correct: {} / Incorrect: {} / {}%", right_count, wrong_count, GetRatio());
}

EFileSaveResult AnswerManager::DumpAnswerDatabaseIntoFile()
{
	if (pAnswerDatabase->GetExcerciseCount() == 0)
		return EFileSaveResult::NothingToSave;
	const auto res1 = FileManager::AddNewRecords(pAnswerDatabase);
	[[maybe_unused]] const auto res2 = FileManager::AddNewRecords(pAnswerDatabase, ELocation::WindowsTempFolder);
	ResetAllCurrentSessionAnswers();
	return res1;
}
