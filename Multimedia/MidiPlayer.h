#pragma once

enum ESpecificNote : int8;

struct MidiPlayer
{
	/* NOTE: playing a new note will stop the previous one */
    static void Play(ESpecificNote note);
    static void Stop();
private:
    MidiPlayer() = delete;
    ~MidiPlayer() = delete;
};
