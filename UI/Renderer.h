#pragma once
#include "pch.h"
#include "app_constants.h"
#include "cute_enums.h"

class QuestionManager;

enum class NoteFeedback : char { None, Correct, Wrong };

struct RenderingState
{
    bool is_excercise_started;
    bool is_start_button_hovered;
    EClefType selected_clef;
    bool is_mixed_clef_enabled;
    NoteFeedback last_answered_note_feedback = NoteFeedback::None;
    ESpecificNote last_answered_note;
    EClefType last_answered_clef;
    const std::vector<wxPoint>& note_positions;
    const QuestionManager& question_manager;
    std::optional<ESpecificNote> external_hovered_specific_note;
    std::optional<EClefType> external_hovered_clef;
    bool has_user_played_today;
    unsigned streak_days_if_practice_today;
};

class Renderer
{
public:
    void RenderMainWindow(wxDC& drawing_context, const RenderingState& rendering_state) const;

private:
    void DrawClef(wxDC& drawing_context, const RenderingState& rendering_state) const;
    void DrawStaff(wxDC& drawing_context) const;
    void DrawAnswerCircles(wxDC& drawing_context, const std::vector<wxPoint>& note_positions) const;
    void DrawQuestions(wxDC& drawing_context, const QuestionManager& question_manager) const;
    void DrawClefChangeMarker(wxDC& drawing_context, int boundary_x, EClefType next_clef) const;
    void DrawNote(wxDC& drawing_context, int x_position, EClefType clef, ESpecificNote note) const;
    void DrawSectionLines(wxDC& drawing_context) const;
    void DrawAnswerFeedback(wxDC& drawing_context, const RenderingState& rendering_state) const;
    void DrawStartButton(wxDC& drawing_context, bool is_start_button_hovered) const;
    void DrawHoverPreview(wxDC& drawing_context, const RenderingState& rendering_state) const;
    void DrawDailyPlayIndicator(wxDC& drawing_context, bool has_played_today, unsigned streak_days_if_practice_today) const;
    void DrawDailyBadgePlayed(wxDC& drawing_context, int center_x, int center_y, int radius) const;
    void DrawDailyBadgeNotPlayed(wxDC& drawing_context, int center_x, int center_y, int radius) const;
    static wxPoint RotateOffset(const wxPoint& offset, double radians);
};
