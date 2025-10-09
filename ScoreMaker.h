#pragma once
#include "Answer.h"
#include "app_constants.h"
class ScoreMaker
{
public:
	using score_t = int64_t;

	static constexpr [[nodiscard]] score_t EvaluateAnswer(const Answer& answer, bool is_repeating = false) noexcept
	{
		const score_t answer_speed = answer.GetTime().count();
		score_t score = 0;
		if (answer.IsCorrect())
		{
			const double factor = is_repeating ? 0.69 : 1;
			score = score_t((-0.2 * answer_speed + SCORE_BASE_TIME_MS) * factor);
		}
		else
		{
			score = score_t(-0.069 * answer_speed - 69.0);
		}
		return score;
	}
};

