#include "pch.h"
#include "UI/WindowMain.h"
#include "cute_enums.h"
#include "Multimedia/MidiPlayer.h"
#include "StatMaker.h"
#include "UI/WindowMotivation.h"
#include <thread>
#include <chrono>

namespace
{
	inline void Sleep(int milliseconds) noexcept
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	}

	void PlayIntroMelody()
	{
		using enum ESpecificNote;

		constexpr int DURATION = 69;

		MidiPlayer::Play(NOTE_B_5); Sleep(DURATION/2);

		MidiPlayer::Play(NOTE_C_6); Sleep(DURATION);
		MidiPlayer::Play(NOTE_E_6); Sleep(DURATION);
		MidiPlayer::Play(NOTE_G_6); Sleep(DURATION);
		MidiPlayer::Play(NOTE_A_6); Sleep(DURATION);

		MidiPlayer::Play(NOTE_C_7); Sleep(DURATION);
		MidiPlayer::Play(NOTE_A_6); Sleep(DURATION);
		MidiPlayer::Play(NOTE_G_6); Sleep(DURATION);
		MidiPlayer::Play(NOTE_E_6); Sleep(DURATION);

		MidiPlayer::Play(NOTE_C_6); Sleep(DURATION * 12);
		MidiPlayer::Stop();
	}
}

struct Main final : wxApp
{
    Main() = default;
    bool OnInit() final
    {
        StatMaker::Refresh();
        m_window = new WindowMain();
        m_window->Show();
        if (WindowMotivation::ShouldShowToday())
        {
			std::thread(&PlayIntroMelody).detach();
            auto* const splash = new WindowMotivation(nullptr, 2);
            splash->Show();
        }
        return true;
    }
private:
    WindowMain* m_window{};
};

wxIMPLEMENT_APP(Main);
