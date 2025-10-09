#pragma once
#include "pch.h"
#include "cute_enums.h"

struct NoteStat
{
	constexpr NoteStat() = default;
	constexpr NoteStat(ESpecificNote SpecificNote, EClefType ClefType, const std::chrono::milliseconds& AvgTime, size_t WrongCount) noexcept :
		specific_note{ SpecificNote },
		clef_type{ ClefType },
		average_time{ AvgTime },
		wrong_count{ WrongCount }
	{ }
	ESpecificNote specific_note{};
	EClefType clef_type{};
	std::chrono::milliseconds average_time{};
	size_t wrong_count{};
};

class StatMaker
{
	static [[nodiscard]] bool Init();
	static [[nodiscard]] std::vector<NoteStat> GetNotes(const std::vector<NoteStat>& notes, size_t count);
public:
	static [[nodiscard]] std::vector<NoteStat> GetSlowestNotes(size_t count);
	static [[nodiscard]] std::vector<NoteStat> GetWrongestNotes(size_t count);
	static [[nodiscard]] std::vector<NoteStat> GetSlowestNotes(EClefType clef, size_t count);
	static [[nodiscard]] std::vector<NoteStat> GetWrongestNotes(EClefType clef, size_t count);
	static [[nodiscard]] bool Refresh();
};
