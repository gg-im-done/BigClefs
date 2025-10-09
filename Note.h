#pragma once
#include "cute_enums.h"

class Note
{
	int8 octave{ 4 };
	ENote note{ ENote::C };
	EClefType clef{ EClefType::Treble };
	ESpecificNote specific_note{ NOTE_A_0 };
public:
	constexpr Note() noexcept = default;

	constexpr explicit Note(ESpecificNote specific_note_, EClefType clef_type) noexcept	:
		note{ static_cast<ENote>(specific_note_ % 7) },
		clef{ clef_type },
		specific_note{ specific_note_ }
	{
		octave = (std::to_underlying(specific_note_) / 7) - (ESpecificNote::NOTE_C_0 / 7);
	}

	constexpr bool operator==(const Note&) const noexcept = default;

	constexpr [[nodiscard]] auto GetSpecificNote() const noexcept
	{
		return specific_note;
	}

	constexpr [[nodiscard]] auto GetClef() const noexcept
	{
		return clef;
	}

	constexpr [[nodiscard]] auto GetNote() const noexcept
	{
		return note;
	}

	constexpr [[nodiscard]] auto GetOctave() const noexcept
	{
		return octave;
	}

	constexpr [[nodiscard]] bool IsNoteEqualsTo(ENote other_note) const noexcept
	{
		return note == other_note;
	}

	constexpr [[nodiscard]] int GetLedgerLine() const noexcept
	{
		//const unsigned nNote = static_cast<unsigned>(note);
		//const unsigned specific_note = nNote + (octave + 2) * 7;

		switch (clef)
		{
			case EClefType::Treble:
			{
				int diff = specific_note - NOTE_A_5;
				if (diff >= 0)
				{
					return 1 + diff / 2;
				}
				diff = specific_note - NOTE_C_4;
				if (diff <= 0)
				{
					return ((-1) + diff / 2);
				}
				break;
			}
			case EClefType::Bass:
			{
				int diff = specific_note - NOTE_C_4;
				if (diff >= 0)
				{
					return 1 + (diff / 2);
				}
				diff = specific_note - NOTE_E_2;
				if (diff <= 0)
				{
					return ((-1) + diff / 2);
				}
				break;
			}
		}

		return 0;
	}
};

