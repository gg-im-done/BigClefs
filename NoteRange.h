#pragma once
#include "cute_enums.h"

struct NoteRange
{
	ESpecificNote min_note{ NOTE_A_3 };
	ESpecificNote max_note{ NOTE_C_6 };

	constexpr NoteRange() noexcept = default;
	constexpr ~NoteRange() noexcept = default;

	constexpr void SetRange(unsigned ledger_lines, EClefType clef) noexcept
	{
		if (constexpr auto MAX_LEDGER_LINES = 4u; ledger_lines > MAX_LEDGER_LINES)
		{
			return;
		}

		switch (clef)
		{
		case EClefType::Treble:
		{
			int min_note_value = NOTE_E_4;
			min_note_value -= ledger_lines * 2;
			min_note = static_cast<ESpecificNote>(min_note_value);
			int max_note_value = NOTE_F_5;
			max_note_value += ledger_lines * 2;
			max_note = static_cast<ESpecificNote>(max_note_value);
			break;
		}
		case EClefType::Bass:
		{
			int min_note_value = NOTE_G_2;
			min_note_value -= ledger_lines * 2;
			min_note = static_cast<ESpecificNote>(min_note_value);
			int max_note_value = NOTE_A_3;
			max_note_value += ledger_lines * 2;
			max_note = static_cast<ESpecificNote>(max_note_value);
			break;
		}
		}//switch
	}

	constexpr explicit NoteRange(unsigned ledger_lines, EClefType clef) noexcept
	{
		this->SetRange(ledger_lines, clef);
	}
};

