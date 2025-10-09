#include "pch.h"
#include "ExerciseResult.h"

ExerciseResult::ExerciseResult(const std::vector<Answer>& new_answers)
	: v_answers{ new_answers }, completion_date{ time(0) }
{
}

ExerciseResult::ExerciseResult(const std::vector<Answer>& new_answers, time_t date_time)
	: v_answers{ new_answers }, completion_date{ date_time }
{
}

auto ExerciseResult::GetStdDateTime() const noexcept -> std::chrono::system_clock::time_point
{
	return std::chrono::system_clock::from_time_t(completion_date);
}
