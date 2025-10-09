#include "pch.h"
#include "AnswerDatabase.h"

AnswersData::~AnswersData() noexcept
{
	//TODO: stats or saving into file
}

void AnswersData::AddExcerciseResult(const ExerciseResult& excercise_result)
{
	v_exercise_results.push_back(excercise_result);
}

void AnswersData::AddExcerciseResult(ExerciseResult&& excercise_result)
{
	v_exercise_results.push_back(std::move(excercise_result));
}
