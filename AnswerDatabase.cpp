#include "pch.h"
#include "AnswerDatabase.h"

AnswersData::~AnswersData() noexcept
{
	//TODO: stats or saving into file
}

void AnswersData::AddExerciseResult(const ExerciseResult& exercise_result)
{
	v_exercise_results.push_back(exercise_result);
}

void AnswersData::AddExerciseResult(ExerciseResult&& exercise_result)
{
	v_exercise_results.push_back(std::move(exercise_result));
}
