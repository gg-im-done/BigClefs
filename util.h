#pragma once
#include "pch.h"
#include "cute_enums.h"
#include "app_constants.h"
#include "NoteRange.h"
#include "Note.h"

constexpr [[nodiscard]] int Square(int number) noexcept
{
    return number * number;
}

inline [[nodiscard]] bool IsPointInCircle(int point_X, int point_Y, int circle_center_X, int circle_center_Y, int circle_radius) noexcept
{
    const auto dist = int(std::sqrt(Square(point_X - circle_center_X) + Square(point_Y - circle_center_Y)));
    return dist <= circle_radius;
}

constexpr [[nodiscard]] int GetNoteYPosition(EClefType clef, ESpecificNote note) noexcept
{
    const int first_line_note = (clef == EClefType::Treble) ? NOTE_E_4 : NOTE_G_2;
    const int noteY = FIRST_CLEF_LINE_BASE_Y - (note - first_line_note) * (NOTE_SPACING);
    return noteY;
}

constexpr [[nodiscard]] bool IsNoteValidForClef(EClefType clef, ESpecificNote note) noexcept
{
    using enum EClefType;
    constexpr NoteRange TREBLE_RANGE(LEDGER_LINES_LIMIT, Treble);
    constexpr NoteRange BASS_RANGE(LEDGER_LINES_LIMIT, Bass);
    switch (clef)
    {
    case Treble:
        return note >= TREBLE_RANGE.min_note && note <= TREBLE_RANGE.max_note;
    case Bass:
        return note >= BASS_RANGE.min_note && note <= BASS_RANGE.max_note;
    default:
        return false;
    }
}

constexpr [[nodiscard]] const char* NoteEnumToString(ENote eNote) noexcept
{
    using enum ENote;
    switch (eNote)
    {
    case C: return "C";
    case D: return "D";
    case E: return "E";
    case F: return "F";
    case G: return "G";
    case A: return "A";
    case B: return "B";
    default: return "?";
    }
}

inline [[nodiscard]] std::string NoteToStringMinimal(Note note)
{
    const auto note_name = NoteEnumToString(note.GetNote());
    const auto octave = static_cast<int>(note.GetOctave());
    return std::format(" {} {} ", note_name, octave);
}
