#include "pch.h"
#include "cute_enums.h"
#include "MidiPlayer.h"
#include "RtMidi.h"

#pragma comment(lib, "Winmm.lib")

namespace
{
	enum EMidiEvent : uint8
	{
		MIDI_EVENT_NoteOff = 0x80,
		MIDI_EVENT_NoteOn = 0x90,
	};

	constexpr int BASE_MIDI_CODE = 12;
	constexpr std::array<int, 7> OFFSETS = { 0, 2, 4, 5, 7, 9, 11 };
	constexpr long long MAX_NOTE_DURATION = 4;
	constexpr unsigned char VELOCITY = 96;

	constexpr unsigned char ConvertToMidiCode(ESpecificNote note) noexcept
	{
		const int octave = (static_cast<int>(note) - static_cast<int>(ESpecificNote::NOTE_C_0)) / 7;
		const int note_index = (static_cast<int>(note) - static_cast<int>(ESpecificNote::NOTE_C_0)) % 7;
		return static_cast<unsigned char>(BASE_MIDI_CODE + (octave * 12) + OFFSETS[note_index]);
	}
	
	std::unique_ptr<RtMidiOut> rt_midi_out;
	auto played_note = ConvertToMidiCode(ESpecificNote::NOTE_A_0);
	long long play_duration = -1;
	std::vector<unsigned char> note_event;
	bool is_midi_api_broken = false;
	bool is_initialized = false;
}

namespace
{
	namespace UT
	{
		consteval void UT_ME_SEMPAI()
		{
			constexpr auto a3 = ESpecificNote::NOTE_A_3;
			constexpr auto res_a3 = ConvertToMidiCode(a3);
			static_assert(res_a3 == 57);
			constexpr auto b4 = ESpecificNote::NOTE_B_4;
			constexpr auto res_b4 = ConvertToMidiCode(b4);
			static_assert(res_b4 == 71);
			constexpr auto c0 = ESpecificNote::NOTE_C_0;
			constexpr auto res_c0 = ConvertToMidiCode(c0);
			static_assert(res_c0 == 12);
		}
	}
}

void MidiPlayer::Play(ESpecificNote note)
{
	if (is_midi_api_broken)
	{
		return;
	}
	if (!is_initialized)
	{
		if (!rt_midi_out)
		{
			try
			{
				rt_midi_out = std::make_unique<RtMidiOut>();
			}
			catch ([[maybe_unused]] const RtMidiError&)
			{
				is_midi_api_broken = true;
				return;
			}
		}
		try
		{
			const unsigned int available_ports = rt_midi_out->getPortCount();
			if (available_ports == 0)
			{
				is_midi_api_broken = true;
				return;
			}
			rt_midi_out->openPort(0);
		}
		catch ([[maybe_unused]] const RtMidiError&)
		{
			is_midi_api_broken = true;
			return;
		}
		if (rt_midi_out->isPortOpen())
		{
			is_initialized = true;
		}
		else
		{
			return;
		}
	}

	if (play_duration != -1)
	{
		note_event.clear();
		note_event.push_back(MIDI_EVENT_NoteOff);
		note_event.push_back(played_note);
		note_event.push_back(0);
		rt_midi_out->sendMessage(&note_event);
	}

	play_duration = 0;//TODO: count seconds and forcibly stop after MAX_NOTE_DURATION seconds
	played_note = ConvertToMidiCode(note);

	note_event.clear();
	note_event.push_back(MIDI_EVENT_NoteOn);
	note_event.push_back(played_note);
	note_event.push_back(VELOCITY);
	rt_midi_out->sendMessage(&note_event);
}

void MidiPlayer::Stop()
{
    if (is_midi_api_broken)
    {
        return;
    }
    if (!is_initialized)
    {
        return;
    }
    if (play_duration == -1)
    {
        return;
    }
    note_event.clear();
    note_event.push_back(MIDI_EVENT_NoteOff);
    note_event.push_back(played_note);
    note_event.push_back(0);
    rt_midi_out->sendMessage(&note_event);
    play_duration = -1;
}
