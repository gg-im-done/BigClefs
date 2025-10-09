#include "pch.h"
#include "DailyStatistics.h"
#include "ExerciseResult.h"
#include "Answer.h"
#include "Note.h"

namespace
{
    struct DailyClefAggregate
    {
        size_t treble_total{ 0 };
        size_t bass_total{ 0 };
        size_t treble_correct{ 0 };
        size_t bass_correct{ 0 };
        size_t treble_time_total_ms{ 0 };
        size_t bass_time_total_ms{ 0 };
        std::string label;
    };

    static std::unordered_map<std::string, DailyClefAggregate> ComputeDailyClefAggregatesByDate(const std::vector<ExerciseResult>& results)
    {
        std::unordered_map<std::string, DailyClefAggregate> aggregates_by_date;
        aggregates_by_date.reserve(results.size());
        for (const auto& result : results)
        {
            const wxDateTime date(result.GetDateTime());
            const std::string key = date.FormatISODate().ToStdString();
            auto& aggregate = aggregates_by_date[key];
            if (aggregate.label.empty())
            {
                aggregate.label = date.Format("%d-%b-%y").ToStdString();
            }
            const auto& answers = result.GetAnswersArrayRef();
            for (const auto& answer : answers)
            {
                const auto clef = answer.GetNote().GetClef();
                const bool is_correct = answer.IsCorrect();
                const size_t time_ms = static_cast<size_t>(answer.GetTime().count());
                if (clef == EClefType::Treble)
                {
                    ++aggregate.treble_total;
                    aggregate.treble_time_total_ms += time_ms;
                    if (is_correct)
                    {
                        ++aggregate.treble_correct;
                    }
                }
                else if (clef == EClefType::Bass)
                {
                    ++aggregate.bass_total;
                    aggregate.bass_time_total_ms += time_ms;
                    if (is_correct)
                    {
                        ++aggregate.bass_correct;
                    }
                }
            }
        }
        return aggregates_by_date;
    }

    static std::vector<std::string> GetDateKeysSortedDescending(const std::unordered_map<std::string, DailyClefAggregate>& aggregates_by_date)
    {
        std::vector<std::string> date_keys;
        date_keys.reserve(aggregates_by_date.size());
        for (const auto& entry : aggregates_by_date)
        {
            date_keys.push_back(entry.first);
        }
        std::ranges::sort(date_keys);
        std::reverse(date_keys.begin(), date_keys.end());
        return date_keys;
    }

    static std::vector<DailyStatisticsRow> ConvertAggregatesToDailyRows(const std::unordered_map<std::string, DailyClefAggregate>& aggregates_by_date, const std::vector<std::string>& date_keys)
    {
        std::vector<DailyStatisticsRow> daily_rows;
        daily_rows.reserve(date_keys.size());
        for (const auto& key : date_keys)
        {
            const auto& aggregate = aggregates_by_date.at(key);
            DailyStatisticsRow row;
            row.date_key = key;
            row.date_label = aggregate.label;
            row.treble_total = aggregate.treble_total;
            row.bass_total = aggregate.bass_total;
            row.treble_accuracy_percent = aggregate.treble_total == 0 ? 0.0 : (static_cast<double>(aggregate.treble_correct) * 100.0 / static_cast<double>(aggregate.treble_total));
            row.bass_accuracy_percent = aggregate.bass_total == 0 ? 0.0 : (static_cast<double>(aggregate.bass_correct) * 100.0 / static_cast<double>(aggregate.bass_total));
            const size_t total_answers = aggregate.treble_total + aggregate.bass_total;
            const size_t total_correct = aggregate.treble_correct + aggregate.bass_correct;
            row.total_answers = total_answers;
            row.overall_accuracy_percent = total_answers == 0 ? 0.0 : (static_cast<double>(total_correct) * 100.0 / static_cast<double>(total_answers));
            row.treble_average_time = aggregate.treble_total == 0 ? std::chrono::milliseconds{ 0 } : std::chrono::milliseconds{ aggregate.treble_time_total_ms / aggregate.treble_total };
            row.bass_average_time = aggregate.bass_total == 0 ? std::chrono::milliseconds{ 0 } : std::chrono::milliseconds{ aggregate.bass_time_total_ms / aggregate.bass_total };
            const size_t overall_time_total_ms = aggregate.treble_time_total_ms + aggregate.bass_time_total_ms;
            row.overall_average_time = total_answers == 0 ? std::chrono::milliseconds{ 0 } : std::chrono::milliseconds{ overall_time_total_ms / total_answers };
            daily_rows.push_back(row);
        }
        return daily_rows;
    }
}

std::vector<DailyStatisticsRow> DailyStatisticsCalculator::Calculate(const std::vector<ExerciseResult>& results)
{
    const auto aggregates_by_date = ComputeDailyClefAggregatesByDate(results);
    const auto date_keys_desc = GetDateKeysSortedDescending(aggregates_by_date);
    return ConvertAggregatesToDailyRows(aggregates_by_date, date_keys_desc);
}
