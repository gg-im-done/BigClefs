#include "pch.h"
#include "WindowMotivation.h"
#include "UI/Events.h"
#include "StatMaker.h"
#include "util.h"

namespace
{
    [[nodiscard]] static std::filesystem::path GetSuppressionFilePathInTempDirectory()
    {
        const auto temp_dir_wpath = wxStandardPaths::Get().GetTempDir().ToStdWstring();
        const std::filesystem::path temp_dir_path(temp_dir_wpath);
        return temp_dir_path / L"wxbigclefs_motivation_suppress";
    }

    [[nodiscard]] static std::string GetLocalDateKeyForToday()
    {
        const wxDateTime today = wxDateTime::Today();
        const int year = today.GetYear();
        const unsigned month = static_cast<unsigned>(today.GetMonth()) + 1;
        const unsigned day = static_cast<unsigned>(today.GetDay());
        return std::format("{:04}-{:02}-{:02}", year, month, day);
    }
}

WindowMotivation::WindowMotivation(wxWindow* parent, size_t worst_notes_count, bool is_suppression_button_visible, bool should_stay_on_top)
    : wxFrame(parent, wxID_ANY, "",  wxDefaultPosition, wxSize(600, 420),
        (wxFRAME_NO_TASKBAR | wxBORDER_NONE) | (should_stay_on_top ? wxSTAY_ON_TOP : 0))
{
    active_window_instance = this;
    SetBackgroundColour(*wxWHITE);
    suppress_for_today_button = new wxButton(this, wxID_ANY, "Don't show today", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    suppress_for_today_button->Bind(wxEVT_BUTTON, &WindowMotivation::OnSuppressForTodayButtonClicked, this);
    BuildContent(worst_notes_count, is_suppression_button_visible);
    Bind(wxEVT_LEFT_DOWN, &WindowMotivation::OnAnyMouseDown, this);
    Bind(wxEVT_RIGHT_DOWN, &WindowMotivation::OnAnyMouseDown, this);
    Bind(wxEVT_MIDDLE_DOWN, &WindowMotivation::OnAnyMouseDown, this);
    Bind(wxEVT_KEY_DOWN, &WindowMotivation::OnKeyHook, this);
    Bind(wxEVT_ACTIVATE, &WindowMotivation::OnActivate, this);
    Bind(wxEVT_ACTIVATE_APP, &WindowMotivation::OnActivateApp, this);
    CentreOnScreen();
}

WindowMotivation::~WindowMotivation()
{
    if (active_window_instance == this)
    {
        active_window_instance = nullptr;
    }
}

/*static*/ bool WindowMotivation::ShouldShowToday()
{
    std::ifstream input_stream(GetSuppressionFilePathInTempDirectory(), std::ios::in | std::ios::binary);
    if (!input_stream.is_open())
    {
        return true;
    }
    std::string stored_date_key;
    std::getline(input_stream, stored_date_key);
    return stored_date_key != GetLocalDateKeyForToday();
}

/*static*/ bool WindowMotivation::IsOpen()
{
    return active_window_instance != nullptr;
}

/*static*/ std::optional<Note> WindowMotivation::GetHoveredNoteIfAny()
{
    if (active_window_instance == nullptr)
    {
        return std::nullopt;
    }
    if (!active_window_instance->HasHoveredNote())
    {
        return std::nullopt;
    }
    return active_window_instance->GetCurrentlyHoveredNote();
}

/*static*/ void WindowMotivation::MarkDismissedToday()
{
    std::ofstream output_stream(GetSuppressionFilePathInTempDirectory(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!output_stream.is_open())
    {
        return;
    }
    const auto today_date_key = GetLocalDateKeyForToday();
    output_stream.write(today_date_key.data(), static_cast<std::streamsize>(today_date_key.size()));
}

/*static*/ void WindowMotivation::RaiseIfOpen()
{
    if (active_window_instance)
    {
        active_window_instance->Show();
        active_window_instance->Raise();
    }
}

wxStaticBoxSizer* WindowMotivation::CreateNotesListStaticBoxSizer(wxWindow* parent, const wxString& box_label, const std::vector<NoteStat>& note_stats, EClefType clef_type)
{
    auto* const static_box = new wxStaticBox(parent, wxID_ANY, box_label);
    auto* const static_box_sizer = new wxStaticBoxSizer(static_box, wxVERTICAL);
    for (const auto& note_stat : note_stats)
    {
        const Note note(note_stat.specific_note, clef_type);
        auto* const label = new wxStaticText(parent, wxID_ANY, NoteToStringMinimal(note));
        {
            auto font = label->GetFont();
            font.SetWeight(wxFONTWEIGHT_NORMAL);
            label->SetFont(font);
            label->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        }
        label->Bind(wxEVT_ENTER_WINDOW, &WindowMotivation::OnNoteLabelEnter, this);
        label->Bind(wxEVT_LEAVE_WINDOW, &WindowMotivation::OnNoteLabelLeave, this);
        label_control_to_note_map.emplace(label, note);
        static_box_sizer->Add(label, 0, wxTOP, 2);
    }
    return static_box_sizer;
}

void WindowMotivation::BuildContent(size_t worst_notes_count, bool is_suppression_button_visible)
{
    auto* const content_panel = new wxPanel(this);

    auto* const root_sizer = new wxBoxSizer(wxVERTICAL);

    auto* const title_text = new wxStaticText(content_panel, wxID_ANY, "Today's Focus");
    auto title_font = title_text->GetFont();
    title_font.MakeBold();
    title_font.Scale(1.2f);
    title_text->SetFont(title_font);

    auto* const notes_statistics_grid_sizer = new wxGridSizer(2, 2, 10, 20);

    auto treble_slowest_notes = StatMaker::GetSlowestNotes(EClefType::Treble, worst_notes_count);
    auto bass_slowest_notes = StatMaker::GetSlowestNotes(EClefType::Bass, worst_notes_count);
    auto treble_wrongest_notes = StatMaker::GetWrongestNotes(EClefType::Treble, worst_notes_count);
    auto bass_wrongest_notes = StatMaker::GetWrongestNotes(EClefType::Bass, worst_notes_count);

    auto* const treble_slowest_sizer = CreateNotesListStaticBoxSizer(content_panel, "Treble: Slowest", treble_slowest_notes, EClefType::Treble);
    auto* const bass_slowest_sizer = CreateNotesListStaticBoxSizer(content_panel, "Bass: Slowest", bass_slowest_notes, EClefType::Bass);
    auto* const treble_wrongest_sizer = CreateNotesListStaticBoxSizer(content_panel, "Treble: Most Mistakes", treble_wrongest_notes, EClefType::Treble);
    auto* const bass_wrongest_sizer = CreateNotesListStaticBoxSizer(content_panel, "Bass: Most Mistakes", bass_wrongest_notes, EClefType::Bass);

    notes_statistics_grid_sizer->Add(treble_slowest_sizer, 1, wxEXPAND);
    notes_statistics_grid_sizer->Add(bass_slowest_sizer, 1, wxEXPAND);
    notes_statistics_grid_sizer->Add(treble_wrongest_sizer, 1, wxEXPAND);
    notes_statistics_grid_sizer->Add(bass_wrongest_sizer, 1, wxEXPAND);

    suppress_for_today_button->Reparent(content_panel);
    if (!is_suppression_button_visible)
    {
        suppress_for_today_button->Hide();
    }

    root_sizer->Add(title_text, 0, wxALIGN_CENTER | wxALL, 10);
    root_sizer->Add(notes_statistics_grid_sizer, 1, wxEXPAND | wxLEFT | wxRIGHT, 20);
    root_sizer->Add(suppress_for_today_button, 0, wxALIGN_CENTER | wxALL, 10);
    if (!is_suppression_button_visible)
    {
        root_sizer->AddSpacer(15);
    }

    content_panel->SetSizer(root_sizer);
    Fit();
}

void WindowMotivation::OnAnyMouseDown(wxMouseEvent& event)
{
    if (suppress_for_today_button->IsShownOnScreen())
    {
        const wxPoint button_screen_pos = suppress_for_today_button->GetScreenPosition();
        const wxSize button_size = suppress_for_today_button->GetSize();
        const wxPoint button_local_pos = ScreenToClient(button_screen_pos);
        const wxRect button_rect(button_local_pos, button_size);
        if (button_rect.Contains(event.GetPosition()))
        {
            event.Skip();
            return;
        }
    }
    Destroy();
}

void WindowMotivation::OnKeyHook(wxKeyEvent& event)
{
    const int key_code = event.GetKeyCode();
    if (key_code == WXK_SPACE || key_code == WXK_RETURN)
    {
        if (wxWindow::FindFocus() == suppress_for_today_button && suppress_for_today_button->IsShownOnScreen())
        {
            event.Skip();
            return;
        }
        Destroy();
        return;
    }
    if (key_code == WXK_ESCAPE)
    {
        Destroy();
        return;
    }
    event.Skip();
}

void WindowMotivation::OnActivate(wxActivateEvent& event)
{
    if (!event.GetActive())
    {
        Destroy();
        return;
    }
    event.Skip();
}

void WindowMotivation::OnActivateApp(wxActivateEvent& event)
{
    if (!event.GetActive())
    {
        Destroy();
        return;
    }
    event.Skip();
}

void WindowMotivation::OnNoteLabelEnter(wxMouseEvent& event)
{
    auto* const label = static_cast<wxStaticText*>(event.GetEventObject());
    if (currently_hovered_label_control != nullptr && currently_hovered_label_control != label)
    {
        ApplyNormalStyleToLabel(currently_hovered_label_control);
    }
    currently_hovered_label_control = label;
    const auto it = label_control_to_note_map.find(label);
    if (it != label_control_to_note_map.end())
    {
        currently_hovered_note = it->second;
    }
    ApplyHoveredStyleToLabel(label);
    if (auto* const top = wxTheApp->GetTopWindow())
    {
        wxCommandEvent evt(EVT_MOTIVATION_HOVER_CHANGED);
        evt.SetEventObject(this);
        wxPostEvent(top, evt);
    }
    event.Skip();
}

void WindowMotivation::OnNoteLabelLeave(wxMouseEvent& event)
{
    auto* const label = static_cast<wxStaticText*>(event.GetEventObject());
    if (label == currently_hovered_label_control)
    {
        ApplyNormalStyleToLabel(label);
        currently_hovered_label_control = nullptr;
    }
    if (auto* const top = wxTheApp->GetTopWindow())
    {
        wxCommandEvent evt(EVT_MOTIVATION_HOVER_CHANGED);
        evt.SetEventObject(this);
        wxPostEvent(top, evt);
    }
    event.Skip();
}

bool WindowMotivation::HasHoveredNote() const
{
    return currently_hovered_label_control != nullptr;
}

void WindowMotivation::ApplyHoveredStyleToLabel(wxStaticText* label)
{
    auto font = label->GetFont();
    font.MakeBold();
    font.SetUnderlined(true);
    label->SetFont(font);
    label->SetForegroundColour(wxColour(0, 102, 204));
}

void WindowMotivation::ApplyNormalStyleToLabel(wxStaticText* label)
{
    auto font = label->GetFont();
    font.SetWeight(wxFONTWEIGHT_NORMAL);
    font.SetUnderlined(false);
    label->SetFont(font);
    label->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
}

void WindowMotivation::OnSuppressForTodayButtonClicked(wxCommandEvent&)
{
    MarkDismissedToday();
    Destroy();
}
