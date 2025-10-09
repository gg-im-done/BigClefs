#pragma once
#include "pch.h"
#include "Note.h"
#include "cute_enums.h"

class ExerciseResult;

struct NoteStatistic
{
	Note note;
	std::chrono::milliseconds average_time{ 0 };
	size_t attempts{ 0 };
	size_t correct{ 0 };

	constexpr NoteStatistic(Note note_, std::chrono::milliseconds time, size_t attempts_, size_t correct_) noexcept
		: note(note_), average_time(time), attempts(attempts_), correct(correct_)
	{
	}

	constexpr [[nodiscard]] double GetAccuracyPercentage() const noexcept
	{
		return attempts == 0 ? 0.0 : (static_cast<double>(correct) / static_cast<double>(attempts)) * 100.0;
	}
};

class NoteStatisticsCalculator
{
public:
	static [[nodiscard]] std::vector<NoteStatistic> CalculateForClef(
		const std::vector<ExerciseResult>& results, 
		EClefType clef
	);

	static [[nodiscard]] std::pair<std::vector<NoteStatistic>, std::vector<NoteStatistic>> CalculateForBothClefs(
		const std::vector<ExerciseResult>& results
	);

private:
	static [[nodiscard]] std::chrono::milliseconds CalculateAverageTime(const std::vector<std::chrono::milliseconds>& times) noexcept;
	static void SortStatisticsByNote(std::vector<NoteStatistic>& stats);
};
