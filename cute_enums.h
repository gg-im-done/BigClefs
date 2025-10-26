#pragma once
using int8 = char;
using uint8 = unsigned char;

enum class EFileSaveResult : uint8
{
    NothingToSave,
    Success,
    CannotCreateFile
};

enum class EClefType : uint8
{
	Treble,
	Bass
};

enum class ENote : uint8
{
    C,D,E,F,G,A,B
};

enum StatusBarIndex : int
{
    STATUSBAR_ID_ANSWERS = 0,
    STATUSBAR_ID_QUESTIONS = 1,
    STATUSBAR_ID_EXERCISES = 2
};

enum ESpecificNote : int8
{
    NOTE_C_0 = 14,
    NOTE_D_0,
    NOTE_E_0,
    NOTE_F_0,
    NOTE_G_0,
    NOTE_A_0,
    NOTE_B_0,

    NOTE_C_1,
    NOTE_D_1,
    NOTE_E_1,
    NOTE_F_1,
    NOTE_G_1,
    NOTE_A_1,
    NOTE_B_1,

    NOTE_C_2,
    NOTE_D_2,
    NOTE_E_2,
    NOTE_F_2,
    NOTE_G_2, // Bass 1
    NOTE_A_2,
    NOTE_B_2,

    NOTE_C_3,
    NOTE_D_3,
    NOTE_E_3,
    NOTE_F_3,
    NOTE_G_3,
    NOTE_A_3, // Bass 5
    NOTE_B_3,

    NOTE_C_4,
    NOTE_D_4,
    NOTE_E_4, // Treble 1
    NOTE_F_4,
    NOTE_G_4,
    NOTE_A_4,
    NOTE_B_4,

    NOTE_C_5,
    NOTE_D_5,
    NOTE_E_5,
    NOTE_F_5, // Treble 5
    NOTE_G_5,
    NOTE_A_5,
    NOTE_B_5,

    NOTE_C_6,
    NOTE_D_6,
    NOTE_E_6,
    NOTE_F_6,
    NOTE_G_6,
    NOTE_A_6,
    NOTE_B_6,

    NOTE_C_7,
    NOTE_D_7,
    NOTE_E_7,
    NOTE_F_7,
    NOTE_G_7,
    NOTE_A_7,
    NOTE_B_7,

    NOTE_C_8
};
