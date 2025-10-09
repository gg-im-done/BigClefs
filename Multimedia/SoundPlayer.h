#pragma once
class SoundPlayer
{
	SoundPlayer() = delete;
	~SoundPlayer() = delete;
public:
	static void WarmUpSoundPlayer();
	static void PlayExcerciseStartSound();
	static void PlaySelectClefSound();
	static void PlayNoteClickSound();
	static void PlayAbandonSound();
};

