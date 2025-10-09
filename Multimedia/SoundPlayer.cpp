#include "pch.h"
#include "SoundPlayer.h"
#pragma comment(lib, "winmm.lib")

namespace
{
    static [[nodiscard]] std::wstring BuildSoundPath(const wchar_t* file_name) noexcept
    {
        const std::filesystem::path executable_path(wxStandardPaths::Get().GetExecutablePath().ToStdWstring());
        const auto base_directory = executable_path.parent_path();

        const std::filesystem::path candidate_in_base = base_directory / file_name;
        if (std::filesystem::exists(candidate_in_base))
        {
            return candidate_in_base.wstring();
        }

        const std::filesystem::path candidate_in_sounds = base_directory / L"sounds" / file_name;
        if (std::filesystem::exists(candidate_in_sounds))
        {
            return candidate_in_sounds.wstring();
        }

        const auto parent_directory = base_directory.parent_path();
        const auto parent_of_parent_directory = parent_directory.parent_path();
        const std::filesystem::path candidate_in_parent_sounds = parent_of_parent_directory / L"sounds" / file_name;
        if (std::filesystem::exists(candidate_in_parent_sounds))
        {
            return candidate_in_parent_sounds.wstring();
        }

        return std::wstring();
    }

    static void PlayIfExists(const wchar_t* file_name) noexcept
    {
        const auto full_path = BuildSoundPath(file_name);
        if (!full_path.empty())
        {
            PlaySoundW(full_path.c_str(), nullptr, SND_FILENAME | SND_ASYNC);
        }
    }
}

void SoundPlayer::PlayExcerciseStartSound()
{
    PlayIfExists(L"start_click.wav");
}

void SoundPlayer::PlaySelectClefSound()
{
    PlayIfExists(L"clef.wav");
}

void SoundPlayer::PlayNoteClickSound()
{
    PlayIfExists(L"click.wav");
}

void SoundPlayer::PlayAbandonSound()
{
    PlayIfExists(L"abandon.wav");
}

void SoundPlayer::WarmUpSoundPlayer()
{
    PlayIfExists(L"click.wav");
}
