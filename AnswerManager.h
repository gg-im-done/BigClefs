#pragma once
#include "pch.h"
#include "AnswerDatabase.h"

class AnswerManager
{
	std::vector<Answer> answers;
	std::shared_ptr<AnswersData> pAnswerDatabase;
	unsigned right_count = 0;
	unsigned wrong_count = 0;

	[[nodiscard]] int GetRatio() const noexcept;
public:
	AnswerManager() noexcept;
	void ClearSession() noexcept;
	void InitExcerciseSession() noexcept;
	void ShutdownExcerciseSession();
	void ResetAllCurrentSessionAnswers();
	void AddNewAnswer(std::vector<Note>::const_iterator note_iter, bool is_correct) noexcept;
	[[nodiscard]] EFileSaveResult DumpAnswerDatabaseIntoFile();
	[[nodiscard]] std::chrono::milliseconds GetAverageAnswerTime() const noexcept;
	[[nodiscard]] auto GetCount() const noexcept { return answers.size(); }
	[[nodiscard]] auto GetSessionsCount() const noexcept { return pAnswerDatabase->GetExcerciseCount(); }
	[[nodiscard]] auto GetAnswerDatabase() const noexcept { return pAnswerDatabase; }
	[[nodiscard]] std::string GetAnswerStatusText() const noexcept;
	
private:
	AnswerManager(const AnswerManager&) = delete;
	AnswerManager(AnswerManager&&) = delete;
	AnswerManager& operator=(const AnswerManager&) = delete;
	AnswerManager& operator=(AnswerManager&&) = delete;
};
