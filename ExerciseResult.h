#pragma once
#include "pch.h"
#include "Answer.h"

class ExerciseResult
{
	std::vector<Answer> v_answers;
	time_t completion_date;
public:
	explicit ExerciseResult(const std::vector<Answer>& new_answers);
	explicit ExerciseResult(const std::vector<Answer>& new_answers, time_t date_time);
	[[nodiscard]] auto GetDateTime() const noexcept { return completion_date; }
	[[nodiscard]] const auto& GetAnswersArrayRef() const noexcept { return v_answers; }
	[[nodiscard]] auto GetStdDateTime() const noexcept -> std::chrono::system_clock::time_point;
};

