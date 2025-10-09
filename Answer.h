#pragma once
#include "Note.h"
#include "pch.h"

class Answer
{
	std::chrono::milliseconds answer_time{ 0 };
	Note note;
	bool is_correct{ false };
public:
	constexpr Answer(Note note_, const std::chrono::milliseconds& answer_speed, bool is_right) noexcept
		: answer_time(answer_speed), note(note_), is_correct(is_right)
	{
	}

	constexpr Answer() noexcept = default;

	constexpr [[nodiscard]] auto GetTime() const noexcept
	{
		return answer_time;
	}

	constexpr [[nodiscard]] auto GetNote() const noexcept
	{
		return note;
	}

	constexpr [[nodiscard]] auto IsCorrect() const noexcept
	{
		return is_correct;
	}
};
