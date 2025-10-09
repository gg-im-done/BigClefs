#include "pch.h"
#include "UI/Renderer.h"
#include "QuestionManager.h"
#include "util.h"

namespace
{
    constexpr std::array<const char*, NOTE_COUNT> NOTE_NAMES = { "C", "D", "E", "F", "G", "A", "B" };

    inline std::string ToString(ESpecificNote note)
    {
        const int octave = (static_cast<int>(note) - static_cast<int>(ESpecificNote::NOTE_C_0)) / 7;
        const int note_index = (static_cast<int>(note) - static_cast<int>(ESpecificNote::NOTE_C_0)) % 7;
        const auto note_name = NOTE_NAMES[note_index];
        return std::format("{}{}", note_name, octave);
    }
}

void Renderer::RenderMainWindow(wxDC& drawing_context, const RenderingState& rendering_state) const
{
    DrawStaff(drawing_context);
    DrawAnswerCircles(drawing_context, rendering_state.note_positions);
    DrawSectionLines(drawing_context);
    if (rendering_state.is_excercise_started)
    {
        DrawQuestions(drawing_context, rendering_state.question_manager);
        DrawAnswerFeedback(drawing_context, rendering_state);
    }
    else
    {
        DrawStartButton(drawing_context, rendering_state.is_start_button_hovered);
    }
    DrawClef(drawing_context, rendering_state);
    DrawHoverPreview(drawing_context, rendering_state);
    DrawDailyPlayIndicator(drawing_context, rendering_state.has_user_played_today, rendering_state.streak_days_if_practice_today);
}

void Renderer::DrawClef(wxDC& drawing_context, const RenderingState& rendering_state) const
{
    static const wxFont CLEF_LABEL_FONT(22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_EXTRALIGHT);
    static constexpr wxCoord CLEF_TEXT_X = LEFT_EDGE_MARGIN_X / 4;
    static constexpr wxCoord CLEF_TEXT_Y = FIRST_LINE_START_POSITION_Y + STAFF_LINE_SPACING * 4;

    drawing_context.SetFont(CLEF_LABEL_FONT);
    if (!rendering_state.is_excercise_started && rendering_state.is_mixed_clef_enabled && !rendering_state.external_hovered_clef.has_value())
    {
        return;
    }
    EClefType clef_to_show = rendering_state.is_excercise_started ? rendering_state.question_manager.GetCurrentClefType() : rendering_state.selected_clef;
    if (rendering_state.external_hovered_clef.has_value())
    {
        clef_to_show = rendering_state.external_hovered_clef.value();
    }
    switch (clef_to_show)
    {
    case EClefType::Treble:
        drawing_context.DrawRotatedText("Treble", CLEF_TEXT_X, CLEF_TEXT_Y, 90.0);
        break;
    case EClefType::Bass:
        drawing_context.DrawRotatedText("Bass", CLEF_TEXT_X, CLEF_TEXT_Y, 90.0);
        break;
    }
}

void Renderer::DrawStaff(wxDC& drawing_context) const
{
    constexpr wxCoord END_COORD_X = LEFT_EDGE_MARGIN_X + STAFF_LINE_WIDTH;
    for (int i = 0; i < STAFF_LINES_COUNT; ++i)
    {
        const wxCoord staff_line_y = FIRST_LINE_START_POSITION_Y + (i * STAFF_LINE_SPACING);
        drawing_context.DrawLine(LEFT_EDGE_MARGIN_X, staff_line_y, END_COORD_X, staff_line_y);
    }
}

void Renderer::DrawAnswerCircles(wxDC& drawing_context, const std::vector<wxPoint>& note_positions) const
{
    static const wxFont ANSWER_NOTE_FONT(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    drawing_context.SetBrush(*wxWHITE_BRUSH);
    drawing_context.SetPen(*wxBLACK_PEN);
    drawing_context.SetFont(ANSWER_NOTE_FONT);

    for (unsigned i = 0; i < NOTE_COUNT; ++i)
    {
        drawing_context.DrawCircle(note_positions[i], ANSWER_CIRCLE_RADIUS);
        drawing_context.DrawText(NOTE_NAMES[i], note_positions[i].x - 9, note_positions[i].y - 15);
    }
}

void Renderer::DrawQuestions(wxDC& drawing_context, const QuestionManager& question_manager) const
{
    const auto [current_question, last_question_in_pack] = question_manager.GetIterators();
    int counter = 0;
    for (NoteIterator question = current_question; question != last_question_in_pack; ++question, ++counter)
    {
        const int ledger_line_start = CURRENT_NOTE_POSITION_X - LEDGER_LINE_WIDTH / 2 + SECTION_WIDTH * counter;
        const int ledger_line_end = ledger_line_start + LEDGER_LINE_WIDTH;
        auto line = question->GetLedgerLine();
        if (line > 0)
        {
            for (int i = 1; i <= line; ++i)
            {
                const wxCoord y = FIRST_LINE_START_POSITION_Y - i * STAFF_LINE_SPACING;
                drawing_context.DrawLine(ledger_line_start, y, ledger_line_end, y);
            }
        }
        else if (line < 0)
        {
            line = -line;
            for (int i = 1; i <= line; ++i)
            {
                const wxCoord y = FIRST_LINE_START_POSITION_Y + 4 * STAFF_LINE_SPACING + i * STAFF_LINE_SPACING;
                drawing_context.DrawLine(ledger_line_start, y, ledger_line_end, y);
            }
        }

        const auto specific_note = question->GetSpecificNote();
        const auto clef = question->GetClef();
        DrawNote(drawing_context, CURRENT_NOTE_POSITION_X + SECTION_WIDTH * counter, clef, specific_note);

        auto next = question;
        ++next;
        if (next != last_question_in_pack)
        {
            const auto next_clef = next->GetClef();
            if (next_clef != clef)
            {
                const int boundary_x = LEFT_EDGE_MARGIN_X + SECTION_WIDTH * (2 + counter);
                DrawClefChangeMarker(drawing_context, boundary_x, next_clef);
            }
        }
    }
}

 

void Renderer::DrawClefChangeMarker(wxDC& drawing_context, int boundary_x, EClefType next_clef) const
{
    static const wxBrush TREBLE_NEXT_BRUSH_GREEN(wxColour(0, 169, 0));
    static const wxBrush BASS_NEXT_BRUSH_BLUE(wxColour(0, 0, 255));
    const wxPen OUTLINE_PEN(*wxBLACK, 2);

    constexpr int TOP_TIP_Y = FIRST_LINE_START_POSITION_Y - 40;
    constexpr int BOTTOM_TIP_Y = FIRST_LINE_START_POSITION_Y + 4 * STAFF_LINE_SPACING + 40;

    constexpr int TRIANGLE_X_SIZE = 22;
    constexpr int TRIANGLE_Y_HALFSIZE = 10;

    const wxBrush marker_brush = (next_clef == EClefType::Treble) ? TREBLE_NEXT_BRUSH_GREEN : BASS_NEXT_BRUSH_BLUE;

    const wxPoint top_triangle_rightward[3] = {
        wxPoint(boundary_x + TRIANGLE_X_SIZE, TOP_TIP_Y),
        wxPoint(boundary_x, TOP_TIP_Y - TRIANGLE_Y_HALFSIZE),
        wxPoint(boundary_x, TOP_TIP_Y + TRIANGLE_Y_HALFSIZE)
    };

    const wxPoint bottom_triangle_rightward[3] = {
        wxPoint(boundary_x + TRIANGLE_X_SIZE, BOTTOM_TIP_Y),
        wxPoint(boundary_x, BOTTOM_TIP_Y - TRIANGLE_Y_HALFSIZE),
        wxPoint(boundary_x, BOTTOM_TIP_Y + TRIANGLE_Y_HALFSIZE)
    };

    const wxPen old_pen = drawing_context.GetPen();
    const wxBrush old_brush = drawing_context.GetBrush();
    const wxFont old_font = drawing_context.GetFont();

    const wxPen vertical_divider_pen(marker_brush.GetColour(), 3);
    drawing_context.SetPen(vertical_divider_pen);
    drawing_context.DrawLine(boundary_x, TOP_TIP_Y, boundary_x, BOTTOM_TIP_Y);

    drawing_context.SetPen(OUTLINE_PEN);
    drawing_context.SetBrush(marker_brush);
    drawing_context.DrawPolygon(3, top_triangle_rightward);
    drawing_context.DrawPolygon(3, bottom_triangle_rightward);

    drawing_context.SetPen(old_pen);
    drawing_context.SetBrush(old_brush);
    drawing_context.SetFont(old_font);
}

void Renderer::DrawNote(wxDC& drawing_context, int x_position, EClefType clef, ESpecificNote note) const
{
    if (!IsNoteValidForClef(clef, note)) [[unlikely]]
    {
        wxMessageBox("Invalid note for the selected clef.");
        return;
    }

    const int y_position = GetNoteYPosition(clef, note);
    if (y_position == -1) [[unlikely]]
    {
        wxMessageBox("Unable to determine note position.");
        return;
    }

    drawing_context.SetBrush(*wxBLACK_BRUSH);
    drawing_context.DrawCircle(wxPoint(x_position, y_position), NOTE_DOT_RADIUS);
}

void Renderer::DrawSectionLines(wxDC& drawing_context) const
{
    drawing_context.SetPen(*wxGREY_PEN);
    for (unsigned i = 1; i < SECTIONS_ON_SCREEN; i++)
    {
        const wxCoord x = LEFT_EDGE_MARGIN_X + SECTION_WIDTH * i;
        drawing_context.DrawLine(x, STAFF_LINE_SPACING,
                    x, STAFF_LINE_SPACING + STAFF_LINE_SPACING * (5 + 6 + 1)
        );
    }
    drawing_context.SetPen(*wxBLACK_PEN);
}

void Renderer::DrawAnswerFeedback(wxDC& drawing_context, const RenderingState& rendering_state) const
{
    static const wxColour FEEDBACK_GREEN(0, 123, 0);
    static const wxColour FEEDBACK_RED(123, 0, 0);
    static const wxFont FEEDBACK_TEXT_FONT(28, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    static const wxFont FEEDBACK_NOTE_FONT(75, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_EXTRABOLD);

    static constexpr int FEEDBACK_DOT_X = LEFT_EDGE_MARGIN_X + SECTION_WIDTH / 2;

    const EClefType clef_for_feedback = rendering_state.last_answered_clef;
    const int y_position = GetNoteYPosition(clef_for_feedback, rendering_state.last_answered_note);
    const auto note_string = ToString(rendering_state.last_answered_note);

    std::random_device random_device;
    std::uniform_int_distribution offset_range(-8, 8);
    std::uniform_int_distribution angle_range_x10(-16, 16);

    const auto random_x_offset = offset_range(random_device);
    const auto random_y_offset = offset_range(random_device);
    const auto random_angle = static_cast<double>(angle_range_x10(random_device));

    switch (rendering_state.last_answered_note_feedback)
    {
    case NoteFeedback::None:
        drawing_context.SetFont(FEEDBACK_TEXT_FONT);
        drawing_context.DrawText(L"Go!!", FEEDBACK_X_POS - 20, FEEDBACK_Y_POS + 20);
        break;
    case NoteFeedback::Correct:
        drawing_context.SetFont(FEEDBACK_TEXT_FONT);
        drawing_context.SetTextForeground(FEEDBACK_GREEN);
        drawing_context.DrawRotatedText(
            note_string,
            FEEDBACK_X_POS + random_x_offset,
            CIRCLE_CENTER_Y + random_y_offset - 69,
            random_angle
        );
        drawing_context.SetBrush(*wxGREEN_BRUSH);
        drawing_context.DrawCircle(wxPoint(FEEDBACK_DOT_X, y_position), NOTE_DOT_RADIUS);
        break;
    case NoteFeedback::Wrong:
        drawing_context.SetFont(FEEDBACK_NOTE_FONT);
        drawing_context.SetTextForeground(FEEDBACK_RED);
        drawing_context.DrawRotatedText(
            note_string,
            FEEDBACK_X_POS + random_x_offset - 25,
            CIRCLE_CENTER_Y + random_y_offset - 69,
            random_angle
        );
        drawing_context.SetBrush(*wxRED_BRUSH);
        drawing_context.DrawCircle(wxPoint(FEEDBACK_DOT_X, y_position), NOTE_DOT_RADIUS);
        break;
    }
}

void Renderer::DrawStartButton(wxDC& drawing_context, bool is_start_button_hovered) const
{
    static const wxColour DARK_RED(69, 6, 9);
    static const wxColour JUST_RED(169, 6, 9);
    const wxPen pen(is_start_button_hovered ? DARK_RED : JUST_RED, 6);
    drawing_context.SetPen(pen);
    drawing_context.SetBrush(JUST_RED);
    drawing_context.DrawCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, START_BUTTON_RADIUS);
}

void Renderer::DrawHoverPreview(wxDC& drawing_context, const RenderingState& rendering_state) const
{
    if (!rendering_state.external_hovered_specific_note.has_value() || !rendering_state.external_hovered_clef.has_value())
    {
        return;
    }
    const auto clef = rendering_state.external_hovered_clef.value();
    const auto note = rendering_state.external_hovered_specific_note.value();
    if (!IsNoteValidForClef(clef, note))
    {
        return;
    }
    static constexpr int HOVER_PREVIEW_DOT_X = LEFT_EDGE_MARGIN_X + SECTION_WIDTH / 2;
    const int y = GetNoteYPosition(clef, note);
    drawing_context.SetBrush(*wxLIGHT_GREY_BRUSH);
    drawing_context.DrawCircle(wxPoint(HOVER_PREVIEW_DOT_X, y), NOTE_DOT_RADIUS);
}

void Renderer::DrawDailyPlayIndicator(wxDC& drawing_context, bool has_played_today, unsigned streak_days_if_practice_today) const
{
    constexpr int BADGE_MARGIN_RIGHT = 24;
    constexpr int BADGE_MARGIN_BOTTOM = 34;
    constexpr int BADGE_RADIUS = 16;
    constexpr int LABEL_OFFSET_Y = 25;

    static const wxFont DAILY_LABEL_FONT(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    const wxPen old_pen = drawing_context.GetPen();
    const wxBrush old_brush = drawing_context.GetBrush();
    const wxFont old_font = drawing_context.GetFont();

    int client_width = 0;
    int client_height = 0;
    drawing_context.GetSize(&client_width, &client_height);
    const int badge_center_x = client_width - BADGE_MARGIN_RIGHT;
    const int badge_center_y = client_height - BADGE_MARGIN_BOTTOM;
    drawing_context.SetFont(DAILY_LABEL_FONT);

    constexpr int TEXT_GAP_TO_BADGE = 10;

    if (has_played_today)
    {
        DrawDailyBadgePlayed(drawing_context, badge_center_x, badge_center_y, BADGE_RADIUS);
        drawing_context.SetTextForeground(wxColour(0, 123, 0));
        const auto label = std::format("Streak: {}", streak_days_if_practice_today);
        int text_width = 0, text_height = 0;
        drawing_context.GetTextExtent(label, &text_width, &text_height);
        const int text_anchor_x = badge_center_x - BADGE_RADIUS - TEXT_GAP_TO_BADGE;
        const int text_x = text_anchor_x - text_width;
        drawing_context.DrawText(label, text_x, client_height - LABEL_OFFSET_Y);
    }
    else
    {
        DrawDailyBadgeNotPlayed(drawing_context, badge_center_x, badge_center_y, BADGE_RADIUS);
        drawing_context.SetTextForeground(wxColour(105, 105, 105));
        const auto label = std::format("Potential streak: {}", streak_days_if_practice_today);
        int text_width = 0, text_height = 0;
        drawing_context.GetTextExtent(label, &text_width, &text_height);
        const int text_anchor_x = badge_center_x - BADGE_RADIUS - TEXT_GAP_TO_BADGE;
        const int text_x = text_anchor_x - text_width;
        drawing_context.DrawText(label, text_x, client_height - LABEL_OFFSET_Y);
    }

    drawing_context.SetPen(old_pen);
    drawing_context.SetBrush(old_brush);
    drawing_context.SetFont(old_font);
}

void Renderer::DrawDailyBadgePlayed(wxDC& drawing_context, int center_x, int center_y, int radius) const
{
    const wxColour BADGE_FILL_COLOR(0, 169, 0);
    const wxColour BADGE_OUTLINE_COLOR(0, 123, 0);

    drawing_context.SetPen(wxPen(BADGE_OUTLINE_COLOR, 2));
    drawing_context.SetBrush(wxBrush(BADGE_FILL_COLOR));
    drawing_context.DrawCircle(wxPoint(center_x, center_y), radius);

    const wxPoint left_ribbon[3] = {
        wxPoint(center_x - 10, center_y + radius - 2),
        wxPoint(center_x - 2, center_y + radius + 12),
        wxPoint(center_x - 2, center_y + radius - 2)
    };
    const wxPoint right_ribbon[3] = {
        wxPoint(center_x + 10, center_y + radius - 2),
        wxPoint(center_x + 2, center_y + radius + 12),
        wxPoint(center_x + 2, center_y + radius - 2)
    };
    drawing_context.DrawPolygon(3, left_ribbon);
    drawing_context.DrawPolygon(3, right_ribbon);

    drawing_context.SetPen(wxPen(*wxWHITE, 3));
    drawing_context.DrawLine(center_x - 6, center_y + 1, center_x - 1, center_y + 6);
    drawing_context.DrawLine(center_x - 1, center_y + 6, center_x + 8, center_y - 6);
}

void Renderer::DrawDailyBadgeNotPlayed(wxDC& drawing_context, int center_x, int center_y, int radius) const
{
    const wxColour BADGE_FILL_COLOR(210, 210, 210);
    const wxColour BADGE_OUTLINE_COLOR(150, 150, 150);

    constexpr double ROTATION_DEGREES = -12.0;
    constexpr double ROTATION_RADIANS = ROTATION_DEGREES * std::numbers::pi / 180.0;

    const std::array<wxPoint, 3> OFFSETS = { wxPoint(12, -12), wxPoint(6, -16), wxPoint(16, -6) };

    drawing_context.SetPen(wxPen(BADGE_OUTLINE_COLOR, 2));
    drawing_context.SetBrush(wxBrush(BADGE_FILL_COLOR));
    drawing_context.DrawCircle(wxPoint(center_x, center_y), radius);

    drawing_context.SetPen(*wxTRANSPARENT_PEN);
    drawing_context.SetBrush(*wxWHITE_BRUSH);
    drawing_context.DrawCircle(wxPoint(center_x + 5, center_y - 2), radius - 6);

    drawing_context.SetBrush(wxBrush(wxColour(230, 230, 230)));

    for (wxPoint offset : OFFSETS)
    {
        const wxPoint rotated = RotateOffset(offset, ROTATION_RADIANS);
        drawing_context.DrawCircle(wxPoint(center_x + rotated.x, center_y + rotated.y), 2);
    }
}

wxPoint Renderer::RotateOffset(const wxPoint& offset, double radians)
{
    const double cosine = std::cos(radians);
    const double sine = std::sin(radians);
    const int dx = static_cast<int>(std::lround(offset.x * cosine - offset.y * sine));
    const int dy = static_cast<int>(std::lround(offset.x * sine + offset.y * cosine));
    return wxPoint(dx, dy);
}
