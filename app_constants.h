#pragma once
// Very basic constants
inline constexpr unsigned NOTE_COUNT = 7;
inline constexpr unsigned LEDGER_LINES_LIMIT = 4;
inline constexpr int SECTIONS_ON_SCREEN = 7;
inline constexpr int STAFF_LINES_COUNT = 5;
// Sizes and layout
inline constexpr int WINDOW_SIZE_X = 820;
inline constexpr int WINDOW_SIZE_Y = 750;
inline constexpr int EDGE_GAP = 10;
inline constexpr int FIRST_LINE_START_POSITION_Y = 100;
inline constexpr int LEFT_EDGE_MARGIN_X = 50;
inline constexpr int NOTE_DOT_RADIUS = 10;
inline constexpr double NOTES_CIRCLE_RADIUS = 120.0;
inline constexpr double ANGLE_STEP = 2.0 * 3.141592653589793 / NOTE_COUNT;
inline constexpr int ANSWER_CIRCLE_RADIUS = 49;
inline constexpr int START_BUTTON_RADIUS = 49;
inline constexpr int CIRCLE_CENTER_X = 400;
inline constexpr int CIRCLE_CENTER_Y = 450;
inline constexpr int LEDGER_LINE_WIDTH = 42;
inline constexpr int STAFF_LINE_SPACING = 20;
inline constexpr int STAFF_LINE_WIDTH = 700;
inline constexpr int SECTION_WIDTH = STAFF_LINE_WIDTH / SECTIONS_ON_SCREEN;
inline constexpr int NOTES_ON_SCREEN = SECTIONS_ON_SCREEN - 1;
inline constexpr int NOTE_SPACING = STAFF_LINE_SPACING / 2;
inline constexpr int FIRST_CLEF_LINE_BASE_Y = 180;
inline constexpr int CURRENT_NOTE_POSITION_X = LEFT_EDGE_MARGIN_X + SECTION_WIDTH + SECTION_WIDTH / 2;
inline constexpr int FEEDBACK_X_POS = LEFT_EDGE_MARGIN_X + SECTION_WIDTH / 2 - 22;
inline constexpr int FEEDBACK_Y_POS = 180;

inline constexpr bool DEFAULT_MIXED_CLEF_ENABLED = true;
inline constexpr bool DEFAULT_MIDI_ENABLED = false;
inline constexpr unsigned DEFAULT_CLEF_SWITCH_MIN_NOTES = 3;
inline constexpr unsigned DEFAULT_CLEF_SWITCH_MAX_NOTES = 20;
inline constexpr unsigned DEFAULT_QUESTIONS_COUNT = 25;

inline constexpr unsigned MAX_QUESTIONS_COUNT = 10000;
inline constexpr unsigned MAX_CLEF_SWITCH_NOTES = 1000;
inline constexpr unsigned MAX_ON_SCREEN_NOTE_COUNT = 30;

inline constexpr float MILLISECONDS_TO_SECONDS_FACTOR = 1000.0f;
inline constexpr double SCORE_BASE_TIME_MS = 1000.0;
