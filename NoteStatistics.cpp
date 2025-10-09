#include "pch.h"
#include "NoteStatistics.h"
#include "ExerciseResult.h"
#include <unordered_map>

std::vector<NoteStatistic> NoteStatisticsCalculator::CalculateForClef(
	const std::vector<ExerciseResult>& results, 
	EClefType clef)
{
	std::unordered_map<ESpecificNote, std::vector<std::chrono::milliseconds>> times;
	std::unordered_map<ESpecificNote, size_t> correct_counts;
	std::unordered_map<ESpecificNote, size_t> total_counts;

	for (const auto& exercise : results)
	{
		for (const auto& answer : exercise.GetAnswersArrayRef())
		{
			const Note note = answer.GetNote();
			if (note.GetClef() != clef)
			{
				continue;
			}

			const ESpecificNote specific_note = note.GetSpecificNote();
			times[specific_note].push_back(answer.GetTime());
			total_counts[specific_note]++;
			if (answer.IsCorrect())
			{
				correct_counts[specific_note]++;
			}
		}
	}

	std::vector<NoteStatistic> stats;
	stats.reserve(times.size());

	for (const auto& [specific_note, note_times] : times)
	{
		if (note_times.empty())
		{
			continue;
		}

		const std::chrono::milliseconds average_time = CalculateAverageTime(note_times);
		const Note note(specific_note, clef);
		const size_t attempts = total_counts[specific_note];
		const size_t correct = correct_counts[specific_note];

		stats.emplace_back(note, average_time, attempts, correct);
	}

	SortStatisticsByNote(stats);
	return stats;
}

std::pair<std::vector<NoteStatistic>, std::vector<NoteStatistic>> NoteStatisticsCalculator::CalculateForBothClefs(
	const std::vector<ExerciseResult>& results)
{
	using enum EClefType;
	return {
		CalculateForClef(results, Treble),
		CalculateForClef(results, Bass)
	};
}

std::chrono::milliseconds NoteStatisticsCalculator::CalculateAverageTime(const std::vector<std::chrono::milliseconds>& times) noexcept
{
	size_t total_ms = 0;
	for (const auto time : times)
	{
		total_ms += time.count();
	}
	return std::chrono::milliseconds(total_ms / times.size());
}

void NoteStatisticsCalculator::SortStatisticsByNote(std::vector<NoteStatistic>& stats)
{
	std::sort(stats.begin(), stats.end(), [](const NoteStatistic& first, const NoteStatistic& second) {
			return first.note.GetSpecificNote() < second.note.GetSpecificNote();
		});
}
