#include "pch.h"
#include "StatMaker.h"
#include "FileManager.h"
#include "AnswerDatabase.h"
#include "Note.h"

namespace
{
	struct NoteStatTmp
	{
		constexpr NoteStatTmp() noexcept = default;
		size_t total_time = 0;
		size_t count = 0;
		size_t wrong_count = 0;
	};
	std::shared_ptr<AnswersData> pAnswerDatabase;
	std::vector<NoteStat> slowest_notes;
	std::vector<NoteStat> wrongest_notes;
}

template<>
struct std::hash<Note>
{
	constexpr size_t operator()(Note map_key) const noexcept
	{
		const size_t shift = (map_key.GetClef() == EClefType::Bass) ? 0ull : 1ull;
		return size_t(map_key.GetSpecificNote()) + shift * 128ull;
	}
};

bool StatMaker::Init()
{
	if (pAnswerDatabase)
		return false;

	pAnswerDatabase = FileManager::ReadDatabase();

	if (!pAnswerDatabase)
		return false;

	std::unordered_map<Note, NoteStatTmp> note_stats;

	decltype(auto) excercise_results = pAnswerDatabase->GetArrayRef();
	for (const auto& excercise_result : excercise_results)
	{
		decltype(auto) answers = excercise_result.GetAnswersArrayRef();
		for (const auto& answer : answers)
		{
			note_stats[answer.GetNote()].count++;
			note_stats[answer.GetNote()].total_time += answer.GetTime().count();
			if (!answer.IsCorrect())
			{
				note_stats[answer.GetNote()].wrong_count++;
			}
		}
	}

	std::vector<NoteStat> final_stats;
	final_stats.reserve(note_stats.size());
	for (const auto&[some_note, note_stat_tmp] : note_stats)
	{
		const std::chrono::milliseconds avg_time{ note_stat_tmp.total_time / note_stat_tmp.count };
		final_stats.emplace_back(some_note.GetSpecificNote(), some_note.GetClef(), avg_time, note_stat_tmp.wrong_count);
	}

	slowest_notes = final_stats;
	wrongest_notes = std::move(final_stats);

	std::ranges::sort(slowest_notes, [](const NoteStat& left, const NoteStat& right) {
		return left.average_time > right.average_time;
		});

	std::ranges::sort(wrongest_notes, [](const NoteStat& left, const NoteStat& right) {
		return left.wrong_count > right.wrong_count;
		});

	return true;
}

std::vector<NoteStat> StatMaker::GetNotes(const std::vector<NoteStat>& notes, size_t count)
{
	if (!pAnswerDatabase)
	{
		const bool is_ok_init = StatMaker::Init();
		if (!is_ok_init)
		{
			return std::vector<NoteStat>();
		}
	}
	count = std::min(count, notes.size());
	return std::vector<NoteStat>(notes.cbegin(), notes.cbegin() + count);
}

std::vector<NoteStat> StatMaker::GetSlowestNotes(size_t count)
{
	return GetNotes(slowest_notes, count);
}

std::vector<NoteStat> StatMaker::GetWrongestNotes(size_t count)
{
	return GetNotes(wrongest_notes, count);
}

std::vector<NoteStat> StatMaker::GetSlowestNotes(EClefType clef, size_t count)
{
	if (!pAnswerDatabase)
	{
		const bool is_ok_init = StatMaker::Init();
		if (!is_ok_init)
		{
			return std::vector<NoteStat>();
		}
	}
	std::vector<NoteStat> filtered;
	filtered.reserve(slowest_notes.size());
	for (const auto& item : slowest_notes)
	{
		if (item.clef_type == clef)
		{
			filtered.push_back(item);
		}
	}
	count = std::min(count, filtered.size());
	return std::vector<NoteStat>(filtered.begin(), filtered.begin() + count);
}

std::vector<NoteStat> StatMaker::GetWrongestNotes(EClefType clef, size_t count)
{
	if (!pAnswerDatabase)
	{
		const bool is_ok_init = StatMaker::Init();
		if (!is_ok_init)
		{
			return std::vector<NoteStat>();
		}
	}
	std::vector<NoteStat> filtered;
	filtered.reserve(wrongest_notes.size());
	for (const auto& item : wrongest_notes)
	{
		if (item.clef_type == clef)
		{
			filtered.push_back(item);
		}
	}
	count = std::min(count, filtered.size());
	return std::vector<NoteStat>(filtered.begin(), filtered.begin() + count);
}

bool StatMaker::Refresh()
{
	pAnswerDatabase.reset();
	slowest_notes.clear();
	wrongest_notes.clear();
	return Init();
}
