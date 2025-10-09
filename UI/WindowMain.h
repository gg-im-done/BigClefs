#pragma once
#include "pch.h"
#include "QuestionManager.h"
#include "app_constants.h"
#include "Renderer.h"
#include "NoteStatistics.h"
#include "DailyStatistics.h"

class WindowSettings;

class WindowMain : public wxFrame
{
    NoteFeedback is_last_answered_note_correct{ NoteFeedback::None };
public:
    WindowMain();
    void SetupStatusBar();
    void SetupMenuBar();
    void ApplySettings(unsigned questions_count, EClefType clef, bool is_mixed_clef, unsigned clef_switch_min_notes, unsigned clef_switch_max_notes, bool is_midi_enabled);
private:
    unsigned configured_questions_count{ DEFAULT_QUESTIONS_COUNT };
    unsigned configured_clef_switch_min_notes{ DEFAULT_CLEF_SWITCH_MIN_NOTES };
    unsigned configured_clef_switch_max_notes{ DEFAULT_CLEF_SWITCH_MAX_NOTES };

    std::vector<wxPoint> note_positions;
private:
    Renderer renderer;
    QuestionManager question_manager;
    bool is_started{ false };
    bool is_start_button_hovered{ false };
    std::optional<ENote> hovered_note;
    EClefType selected_clef{ EClefType::Treble };
    bool is_mixed_clef_enabled{ DEFAULT_MIXED_CLEF_ENABLED };
    bool is_midi_enabled{ DEFAULT_MIDI_ENABLED };

    ESpecificNote last_answered_note{};
    EClefType last_answered_clef{ EClefType::Treble };
    WindowSettings* settings_window{};
    bool has_user_played_today{ false };
    unsigned streak_days_if_practice_today{ 1 };
private:
    void StartExcercise();
    void FinishExcercise();
    void UpdateStatusText();
    void UpdateDailyPlayStatus();
private:
    void OnPaint(wxPaintEvent&);
    void OnMouseClick(wxMouseEvent&);
    void OnMouseRightClick(wxMouseEvent&);
    void OnMouseMove(wxMouseEvent&);
    void OnMenuSettings(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuAbortExcercise(wxCommandEvent&);
    void OnMenuSaveSession(wxCommandEvent&);
    void OnMenuForgetSession(wxCommandEvent&);
    void OnMenuShowAllStats(wxCommandEvent&);
    void OnMenuShowMotivation(wxCommandEvent&);
    void OnMenuShowDailyStats(wxCommandEvent&);
    void OnMotivationHoverChanged(wxCommandEvent&);
private:
    [[nodiscard]] std::vector<ExerciseResult> LoadAllExerciseResults() const;
    void ShowNoDataMessage() const;
    void CreateAndShowStatisticsWindow(const std::vector<NoteStatistic>& treble_stats, const std::vector<NoteStatistic>& bass_stats);
    void CreateAndShowDailyStatisticsWindow(const std::vector<DailyStatisticsRow>& rows);
    void OnKeyDown(wxKeyEvent&);
    void OnStartButtonClicked();
    void OnClose(wxCloseEvent&);
private:
    enum ClickResult : unsigned { NOT_CLICKED_ON_NOTE = 69 };
private:
    wxMenuItem* stop_excercise_menu_item{ nullptr };
    wxBitmap stop_excercise_bitmap;
    void EnableStopExcercise();
    void DisableStopExcercise();

    [[nodiscard]] std::optional<ENote> GetNoteAtPoint(wxPoint position) const noexcept;

    wxDECLARE_EVENT_TABLE();
};
