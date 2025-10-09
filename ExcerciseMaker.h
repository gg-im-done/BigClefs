#pragma once
#include "pch.h"
#include "app_constants.h"

class Note;
enum class EClefType : uint8;

class ExcerciseMaker
{
    unsigned questions_count = DEFAULT_QUESTIONS_COUNT;
    unsigned clef_switch_frequency_min = DEFAULT_CLEF_SWITCH_MIN_NOTES;
    unsigned clef_switch_frequency_max = DEFAULT_CLEF_SWITCH_MAX_NOTES;
    unsigned ledger_lines = 2;
    std::optional<EClefType> clef_type;
    bool allow_repeating_notes = true;
private:
    std::vector<Note> GenerateExcerciseOneClef(EClefType clef) const;
public:
    ExcerciseMaker& SetQuestionsCount(unsigned count);
    ExcerciseMaker& SetMinSwitchFreqency(unsigned freq_min);
    ExcerciseMaker& SetMaxSwitchFreqency(unsigned freq_max);
    ExcerciseMaker& SetLedgerLines(unsigned ledger_lines_used);
    ExcerciseMaker& SetClef(EClefType);
    ExcerciseMaker& WithRepeatingNotes(bool);

    [[nodiscard]] std::vector<Note> Generate() const;
};

