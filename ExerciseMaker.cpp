#include "pch.h"
#include "Note.h"
#include "ExerciseMaker.h"
#include "NoteRange.h"

ExerciseMaker& ExerciseMaker::SetQuestionsCount(unsigned count)
{
    questions_count = count;
    return *this;
}

ExerciseMaker& ExerciseMaker::SetMinSwitchFreqency(unsigned freq_min)
{
    clef_switch_frequency_min = freq_min;
    return *this;
}

ExerciseMaker& ExerciseMaker::SetMaxSwitchFreqency(unsigned freq_max)
{
    clef_switch_frequency_max = freq_max;
    return *this;
}

ExerciseMaker& ExerciseMaker::SetLedgerLines(unsigned ledger_lines_used)
{
    ledger_lines = ledger_lines_used;
    return *this;
}

ExerciseMaker& ExerciseMaker::SetClef(EClefType clef)
{
    clef_type = clef;
    return *this;
}

ExerciseMaker& ExerciseMaker::WithRepeatingNotes(bool is_repeating_notes_allowed)
{
    allow_repeating_notes = is_repeating_notes_allowed;
    return *this;
}


std::vector<Note> ExerciseMaker::GenerateExerciseOneClef(EClefType clef) const
{
    std::vector<Note> exercise;
    exercise.reserve(questions_count);
    std::random_device generator;

    const NoteRange note_range(ledger_lines, clef);
    const unsigned min_note_ = note_range.min_note;
    const unsigned max_note_ = note_range.max_note;
    std::uniform_int_distribution rand_range(min_note_, max_note_);

    if (allow_repeating_notes)
    {
        for (unsigned i = 0; i < questions_count; i++)
        {
            const auto specific_note = static_cast<ESpecificNote>(rand_range(generator));
            exercise.emplace_back(specific_note, clef);
        }
    }
    else
    {
        const auto first_note = static_cast<ESpecificNote>(rand_range(generator));
        exercise.emplace_back(first_note, clef);
        for (unsigned i = 1; i < questions_count; i++)
        {
            ESpecificNote specific_note;
            do
            {
                specific_note = static_cast<ESpecificNote>(rand_range(generator));
            } while (specific_note == exercise.back().GetSpecificNote());
            exercise.emplace_back(specific_note, clef);
        }
    }
    return exercise;
}

std::vector<Note> ExerciseMaker::Generate() const
{
    if (clef_type.has_value())
    {
        return GenerateExerciseOneClef(clef_type.value());
    }
    else // Mixed-clef mode
    {
        std::vector<Note> exercise;
        exercise.reserve(questions_count);

        std::random_device generator;
        std::uniform_int_distribution streak_dist(clef_switch_frequency_min, clef_switch_frequency_max);
        std::uniform_int_distribution start_clef_dist(0, 1);

        EClefType current_clef = start_clef_dist(generator) == 0 ? EClefType::Treble : EClefType::Bass;
        unsigned remaining = questions_count;

        ESpecificNote last_specific_note = static_cast<ESpecificNote>(-1);

        while (remaining > 0)
        {
            const unsigned streak_len = std::min<unsigned>(remaining, static_cast<unsigned>(streak_dist(generator)));

            const NoteRange note_range(ledger_lines, current_clef);
            const unsigned min_note_value = note_range.min_note;
            const unsigned max_note_value = note_range.max_note;
            std::uniform_int_distribution note_dist(min_note_value, max_note_value);

            for (unsigned i = 0; i < streak_len; ++i)
            {
                ESpecificNote specific_note;
                if (allow_repeating_notes)
                {
                    specific_note = static_cast<ESpecificNote>(note_dist(generator));
                }
                else
                {
                    // Avoid repeating the same specific note as the last one
                    do
                    {
                        specific_note = static_cast<ESpecificNote>(note_dist(generator));
                    } while (!exercise.empty() && specific_note == last_specific_note);
                }

                exercise.emplace_back(specific_note, current_clef);
                last_specific_note = specific_note;
            }

            remaining -= streak_len;
            current_clef = (current_clef == EClefType::Treble) ? EClefType::Bass : EClefType::Treble;
        }

        return exercise;
    }

}
