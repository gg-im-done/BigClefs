#pragma once
#include "pch.h"
#include "app_constants.h"

class Note;
enum class EClefType : uint8;

class ExerciseMaker
{
    unsigned questions_count = DEFAULT_QUESTIONS_COUNT;
    unsigned clef_switch_frequency_min = DEFAULT_CLEF_SWITCH_MIN_NOTES;
    unsigned clef_switch_frequency_max = DEFAULT_CLEF_SWITCH_MAX_NOTES;
    unsigned ledger_lines = 2;
    std::optional<EClefType> clef_type;
    bool allow_repeating_notes = true;
private:
    std::vector<Note> GenerateExerciseOneClef(EClefType clef) const;
public:
    ExerciseMaker& SetQuestionsCount(unsigned count);
    ExerciseMaker& SetMinSwitchFreqency(unsigned freq_min);
    ExerciseMaker& SetMaxSwitchFreqency(unsigned freq_max);
    ExerciseMaker& SetLedgerLines(unsigned ledger_lines_used);
    ExerciseMaker& SetClef(EClefType);
    ExerciseMaker& WithRepeatingNotes(bool);

    [[nodiscard]] std::vector<Note> Generate() const;
};

