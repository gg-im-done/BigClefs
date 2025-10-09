#pragma once
#include "pch.h"

class ExerciseResult;

struct DailyStatisticsRow
{
    std::string date_label;
    std::string date_key;
    size_t treble_total{ 0 };
    size_t bass_total{ 0 };
    double treble_accuracy_percent{ 0.0 };
    double bass_accuracy_percent{ 0.0 };
    size_t total_answers{ 0 };
    double overall_accuracy_percent{ 0.0 };
    std::chrono::milliseconds treble_average_time{ 0 };
    std::chrono::milliseconds bass_average_time{ 0 };
    std::chrono::milliseconds overall_average_time{ 0 };
};

class DailyStatisticsCalculator
{
public:
    static [[nodiscard]] std::vector<DailyStatisticsRow> Calculate(const std::vector<ExerciseResult>& results);
};
