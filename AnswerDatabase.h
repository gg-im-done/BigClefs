#pragma once
#include "pch.h"
#include "ExerciseResult.h"

class AnswersData
{
	std::vector<ExerciseResult> v_exercise_results;
public:
	AnswersData() = default;
	~AnswersData() noexcept;
public:
	void AddExerciseResult(const ExerciseResult& exercise_result);
	void AddExerciseResult(ExerciseResult&& exercise_result);
	[[nodiscard]] const auto& GetArrayRef() const noexcept { return v_exercise_results; }
	[[nodiscard]] auto GetExerciseCount() const noexcept { return v_exercise_results.size(); }
private:
	AnswersData(const AnswersData&) = delete;
	AnswersData(AnswersData&&) = delete;
	AnswersData& operator=(const AnswersData&) = delete;
	AnswersData& operator=(AnswersData&&) = delete;
};

