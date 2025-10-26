#include "pch.h"
#include "UI/WindowMain.h"
#include "UI/WindowSettings.h"
#include "UI/WindowStatistics.h"
#include "UI/Events.h"
#include "UI/WindowMotivation.h"
#include "Multimedia/SoundPlayer.h"
#include "util.h"
#include "FileManager.h"
#include "NoteStatistics.h"
#include "StatMaker.h"
#include "Multimedia/MidiPlayer.h"
#include "DailyStatistics.h"
#include "UI/WindowDailyStats.h"

wxBEGIN_EVENT_TABLE(WindowMain, wxFrame)
EVT_PAINT(WindowMain::OnPaint)
EVT_LEFT_DOWN(WindowMain::OnMouseClick)
EVT_RIGHT_DOWN(WindowMain::OnMouseRightClick)
EVT_MOTION(WindowMain::OnMouseMove)
EVT_KEY_DOWN(WindowMain::OnKeyDown)
EVT_CLOSE(WindowMain::OnClose)
//EVT_LEFT_DCLICK(WindowMain::OnMouseClick)
// NOTE - we are not supporting repeating notes anymore - the event is disabled to prevent misclicks
wxEND_EVENT_TABLE()



namespace
{
    constexpr int TOOLBAR_ICON_SIZE = 24;

    enum class EToolBarItem : unsigned char
    {
        Stop,
        Save,
        Forget,
        Settings,
        AllStats,
        Motivation,
        DailyStats
    };

    enum class EToolBarItemState : unsigned char
    {
        Enabled,
        Disabled
    };

    enum class EToolBarItemType : unsigned char
    {
        Tool,
        Separator
    };

    struct ToolBarItemConfig
    {
        std::optional<wxArtID> art_id;
        wxString label;
        wxString short_help;
        std::optional<wxBitmap> custom_bitmap;
        std::optional<EToolBarItem> item;
        EToolBarItemType type;
        EToolBarItemState initial_state;

        static ToolBarItemConfig CreateWithArt(EToolBarItem item, const wxString& label, const wxArtID& art_id, const wxString& short_help, EToolBarItemState initial_state = EToolBarItemState::Enabled)
        {
            return ToolBarItemConfig{ art_id, label, short_help, std::nullopt, item, EToolBarItemType::Tool, initial_state };
        }

        static ToolBarItemConfig CreateWithBitmap(EToolBarItem item, const wxString& label, const wxBitmap& bitmap, const wxString& short_help, EToolBarItemState initial_state = EToolBarItemState::Enabled)
        {
            return ToolBarItemConfig{ std::nullopt, label, short_help, bitmap, item, EToolBarItemType::Tool, initial_state };
        }

        static ToolBarItemConfig CreateSeparator()
        {
            return ToolBarItemConfig{ std::nullopt, "", "", std::nullopt, std::nullopt, EToolBarItemType::Separator, EToolBarItemState::Enabled };
        }
    };

    [[nodiscard]] std::vector<ToolBarItemConfig> CreateToolBarConfiguration()
    {
        return {
            ToolBarItemConfig::CreateWithArt(EToolBarItem::Stop, "Stop", wxART_STOP, "Stop current exercise", EToolBarItemState::Disabled),
            ToolBarItemConfig::CreateSeparator(),
            ToolBarItemConfig::CreateWithArt(EToolBarItem::Save, "Save", wxART_FILE_SAVE, "Save session results"),
            ToolBarItemConfig::CreateWithArt(EToolBarItem::Forget, "Forget", wxART_UNDO, "Forget session results"),
            ToolBarItemConfig::CreateSeparator(),
            ToolBarItemConfig::CreateWithArt(EToolBarItem::Settings, "Settings", wxART_REPORT_VIEW, "Open settings"),
            ToolBarItemConfig::CreateSeparator(),
            ToolBarItemConfig::CreateWithArt(EToolBarItem::AllStats, "All Stats", wxART_INFORMATION, "Show all statistics"),
            ToolBarItemConfig::CreateWithArt(EToolBarItem::Motivation, "Motivation", wxART_WARNING, "Show motivation window"),
            ToolBarItemConfig::CreateWithArt(EToolBarItem::DailyStats, "Daily Stats", wxART_LIST_VIEW, "Show daily statistics")
        };
    }
}//namespace



WindowMain::WindowMain()
    : wxFrame(nullptr, wxID_ANY, "Amazing Big Clefs", wxDefaultPosition, wxSize(WINDOW_SIZE_X, WINDOW_SIZE_Y), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    //SoundPlayer::WarmUpSoundPlayer();
    Centre();

    note_positions.reserve(NOTE_COUNT);
    for (unsigned i = 0; i < NOTE_COUNT; ++i)
    {
        const double angle = i * ANGLE_STEP - (std::numbers::pi / 2.0);
        const int x = CIRCLE_CENTER_X + static_cast<int>(NOTES_CIRCLE_RADIUS * cos(angle));
        const int y = CIRCLE_CENTER_Y + static_cast<int>(NOTES_CIRCLE_RADIUS * sin(angle));
        note_positions.emplace_back(x, y);
    }

    question_manager.SetOnScreenNoteCount(NOTES_ON_SCREEN);

    SetupToolBar();
    SetupStatusBar();
    auto settings_callback = [this](unsigned questions_count, EClefType clef, bool is_mixed_clef, unsigned clef_switch_min_notes, unsigned clef_switch_max_notes, bool is_midi_enabled) {
        ApplySettings(questions_count, clef, is_mixed_clef, clef_switch_min_notes, clef_switch_max_notes, is_midi_enabled);
    };
    settings_window = new WindowSettings(this, settings_callback);
    UpdateStatusText();
    SetIcon(wxArtProvider::GetIcon(wxART_HELP_BOOK, wxART_FRAME_ICON));

    SetDoubleBuffered(true);
    
    Bind(EVT_MOTIVATION_HOVER_CHANGED, &WindowMain::OnMotivationHoverChanged, this);
    UpdateDailyPlayStatus();
}

void WindowMain::SetupStatusBar()
{
    constexpr auto STATUSBAR_STYLE = 0;
    CreateStatusBar(3, STATUSBAR_STYLE);
    SetStatusText("~answers~info~", STATUSBAR_ID_ANSWERS);
    SetStatusText("~questions~info~", STATUSBAR_ID_QUESTIONS);
    SetStatusText("~exercises~info~", STATUSBAR_ID_EXERCISES);
}

void WindowMain::ApplySettings(unsigned questions_count, EClefType clef, bool is_mixed_clef, unsigned clef_switch_min_notes, unsigned clef_switch_max_notes, bool is_midi_on)
{
    configured_questions_count = questions_count;
    selected_clef = clef;
    is_mixed_clef_enabled = is_mixed_clef;
    if (clef_switch_min_notes <= clef_switch_max_notes)
    {
        configured_clef_switch_min_notes = clef_switch_min_notes;
        configured_clef_switch_max_notes = clef_switch_max_notes;
    }
    else
    {
        configured_clef_switch_min_notes = clef_switch_max_notes;
        configured_clef_switch_max_notes = clef_switch_min_notes;
    }
    is_midi_enabled = is_midi_on;
    Refresh(false);
}

void WindowMain::SetupToolBar()
{
    constexpr auto TOOLBAR_STYLE = wxTB_HORIZONTAL | wxTB_TEXT | wxTB_FLAT | wxTB_NODIVIDER;
    toolbar = CreateToolBar(TOOLBAR_STYLE);
    toolbar->SetToolBitmapSize(wxSize(TOOLBAR_ICON_SIZE, TOOLBAR_ICON_SIZE));

    const auto tool_config = CreateToolBarConfiguration();
    std::unordered_map<EToolBarItem, int> tool_id_map;
    tool_id_map.reserve(tool_config.size());
    for (const auto& config : tool_config)
    {
        if (config.type == EToolBarItemType::Separator)
        {
            toolbar->AddSeparator();
            continue;
        }

        wxBitmap icon;
        if (config.custom_bitmap.has_value() && config.custom_bitmap->IsOk())
        {
            icon = *config.custom_bitmap;
        }
        else
        {
            icon = wxArtProvider::GetBitmap(config.art_id.value(), wxART_TOOLBAR, wxSize(TOOLBAR_ICON_SIZE, TOOLBAR_ICON_SIZE));
        }
        
        const int tool_id = toolbar->AddTool(wxID_ANY, config.label, icon, config.short_help)->GetId();
        
        if (config.initial_state == EToolBarItemState::Disabled)
        {
            toolbar->EnableTool(tool_id, false);
        }

        tool_id_map[config.item.value()] = tool_id;
    }

    toolbar->Realize();

    stop_exercise_tool_id = tool_id_map[EToolBarItem::Stop];

    Bind(wxEVT_TOOL, &WindowMain::OnToolAbortExercise, this, tool_id_map[EToolBarItem::Stop]);
    Bind(wxEVT_TOOL, &WindowMain::OnToolSaveSession, this, tool_id_map[EToolBarItem::Save]);
    Bind(wxEVT_TOOL, &WindowMain::OnToolForgetSession, this, tool_id_map[EToolBarItem::Forget]);
    Bind(wxEVT_TOOL, &WindowMain::OnToolSettings, this, tool_id_map[EToolBarItem::Settings]);
    Bind(wxEVT_TOOL, &WindowMain::OnToolShowAllStats, this, tool_id_map[EToolBarItem::AllStats]);
    Bind(wxEVT_TOOL, &WindowMain::OnToolShowMotivation, this, tool_id_map[EToolBarItem::Motivation]);
    Bind(wxEVT_TOOL, &WindowMain::OnToolShowDailyStats, this, tool_id_map[EToolBarItem::DailyStats]);
}

void WindowMain::StartExercise()
{
    is_started = true;
    const auto questions_count = configured_questions_count;
    if (is_mixed_clef_enabled)
    {
        question_manager.GenerateQuestionsMixed(questions_count, configured_clef_switch_min_notes, configured_clef_switch_max_notes);
    }
    else
    {
        question_manager.GenerateQuestions(questions_count, selected_clef);
    }
}

void WindowMain::FinishExercise()
{
    is_started = false;
    is_last_answered_note_correct = ENoteFeedback::None;
    SetStopExerciseEnabled(false);

    const auto average_ms = question_manager.GetAverageAnswerTime();
    const auto average_seconds = float(average_ms.count()) / MILLISECONDS_TO_SECONDS_FACTOR;
    const auto msg = std::format("Average time: {:.02f}", average_seconds);

    wxMessageBox(msg, "Result", wxOK | wxICON_QUESTION, this);
    UpdateDailyPlayStatus();
}

void WindowMain::UpdateStatusText()
{
    const auto [questions_status, answers_status, exercises_status] = question_manager.GetStatusText();
    SetStatusText(answers_status, STATUSBAR_ID_ANSWERS);
    SetStatusText(questions_status, STATUSBAR_ID_QUESTIONS);
    SetStatusText(exercises_status, STATUSBAR_ID_EXERCISES);
}

void WindowMain::OnPaint(wxPaintEvent&)
{
    wxPaintDC drawing_context(this);
    const auto hovered_from_motivation = WindowMotivation::GetHoveredNoteIfAny();
    const std::optional<ESpecificNote> external_hovered_specific_note = hovered_from_motivation.has_value() ? std::optional<ESpecificNote>(hovered_from_motivation->GetSpecificNote()) : std::nullopt;
    const std::optional<EClefType> external_hovered_clef = hovered_from_motivation.has_value() ? std::optional<EClefType>(hovered_from_motivation->GetClef()) : std::nullopt;
    const RenderingState rendering_state{
        .is_exercise_started = is_started,
        .is_start_button_hovered = is_start_button_hovered,
        .selected_clef = selected_clef,
        .is_mixed_clef_enabled = is_mixed_clef_enabled,
        .last_answered_note_feedback = is_last_answered_note_correct,
        .last_answered_note = last_answered_note,
        .last_answered_clef = last_answered_clef,
        .note_positions = note_positions,
        .question_manager = question_manager,
        .external_hovered_specific_note = external_hovered_specific_note,
        .external_hovered_clef = external_hovered_clef,
        .has_user_played_today = has_user_played_today,
        .streak_days_if_practice_today = streak_days_if_practice_today
    };
    renderer.RenderMainWindow(drawing_context, rendering_state);
}

void WindowMain::OnMouseClick(wxMouseEvent& event)
{
    const wxPoint click_point = event.GetPosition();
    if (!is_started)
    {
        if (IsPointInCircle(click_point.x, click_point.y, CIRCLE_CENTER_X, CIRCLE_CENTER_Y, START_BUTTON_RADIUS))
        {
            OnStartButtonClicked();
        }
        return;
    }

    const auto clicked_note = GetNoteAtPoint(click_point);
    if (!clicked_note.has_value())
        return;

    const auto specific_note = question_manager.GetCurrentQuestionSpecificNote();
    const auto current_clef = question_manager.GetCurrentClefType();
    if (question_manager.TakeAnswer(clicked_note.value()))
    {
        is_last_answered_note_correct = ENoteFeedback::Correct;
        if (specific_note.has_value() && is_midi_enabled)
        {
            MidiPlayer::Play(specific_note.value());
        }
        else
        {
        }
    }
    else
    {
        SoundPlayer::PlayNoteClickSound();
        is_last_answered_note_correct = ENoteFeedback::Wrong;
    }
    last_answered_clef = current_clef;
    last_answered_note = specific_note.value_or(ESpecificNote::NOTE_C_0);

    UpdateStatusText();
    Refresh();
    if (question_manager.IsEnd())
        FinishExercise();
}

void WindowMain::OnMouseRightClick(wxMouseEvent&)
{
    wxCommandEvent whatever(wxEVT_TOOL);
    OnToolAbortExercise(whatever);
}

void WindowMain::OnMouseMove(wxMouseEvent& event)
{
    if (is_started)
        return;

    if (IsPointInCircle(event.GetX(), event.GetY(), CIRCLE_CENTER_X, CIRCLE_CENTER_Y, START_BUTTON_RADIUS))
    {
        if (!is_start_button_hovered)
        {
            is_start_button_hovered = true;
            Refresh(false);
        }
    }
    else
    {
        if (is_start_button_hovered)
        {
            is_start_button_hovered = false;
            Refresh(false);
        }
    }
}

void WindowMain::OnToolSettings(wxCommandEvent&)
{
    settings_window->Show();
    settings_window->Raise();
}

void WindowMain::OnToolAbortExercise(wxCommandEvent&)
{
    if (!is_started)
        return;
    SoundPlayer::PlayAbandonSound();
    is_started = false;
    is_last_answered_note_correct = ENoteFeedback::None;
    question_manager.AbortExercise();
    UpdateStatusText();
    UpdateDailyPlayStatus();
    SetStopExerciseEnabled(false);
    Refresh();
}

void WindowMain::OnToolSaveSession(wxCommandEvent&)
{
    const EFileSaveResult result = question_manager.SaveExerciseStats();
    switch (result)
    {
    case EFileSaveResult::NothingToSave:
        wxMessageBox("Nothing to save...");
        break;
    case EFileSaveResult::Success:
        break;
    case EFileSaveResult::CannotCreateFile:
        wxMessageBox("Error opening data file", "Error", wxOK | wxICON_ERROR);
        break;
    }
    UpdateStatusText();
    UpdateDailyPlayStatus();
    Refresh();
}

void WindowMain::OnToolForgetSession(wxCommandEvent&)
{
    const auto exerciseCount = question_manager.GetAnswerManager()->GetAnswerDatabase()->GetExerciseCount();
    if (exerciseCount == 0)
    {
        return;
    }
    
    const int res = wxMessageBox(std::format("Forget {} exercise results?", exerciseCount), "Confirm", wxYES_NO | wxICON_QUESTION, this);
    if (res != wxYES)
    {
        return;
    }

    question_manager.ResetAllSessionAnswers();
    UpdateStatusText();
    UpdateDailyPlayStatus();
    Refresh();
}

void WindowMain::OnToolShowAllStats(wxCommandEvent&)
{
    const auto all_results = LoadAllExerciseResults();
    if (all_results.empty())
    {
        ShowNoDataMessage();
        return;
    }
    
    const auto [treble_stats, bass_stats] = NoteStatisticsCalculator::CalculateForBothClefs(all_results);
    CreateAndShowStatisticsWindow(treble_stats, bass_stats);
}

void WindowMain::OnToolShowMotivation(wxCommandEvent&)
{
    if (WindowMotivation::IsOpen())
    {
        WindowMotivation::RaiseIfOpen();
        return;
    }
    StatMaker::Refresh();
    auto* const splash = new WindowMotivation(nullptr, 2, false, false);
    splash->Show();
}

void WindowMain::OnToolShowDailyStats(wxCommandEvent&)
{
    const auto all_results = LoadAllExerciseResults();
    if (all_results.empty())
    {
        ShowNoDataMessage();
        return;
    }
    const auto rows = DailyStatisticsCalculator::Calculate(all_results);
    if (rows.empty())
    {
        ShowNoDataMessage();
        return;
    }
    CreateAndShowDailyStatisticsWindow(rows);
}

void WindowMain::OnMotivationHoverChanged(wxCommandEvent&)
{
    Refresh(true);
}

std::vector<ExerciseResult> WindowMain::LoadAllExerciseResults() const
{
    try
    {
        const auto saved_data = FileManager::ReadDatabase();
        const auto session_data = question_manager.GetAnswerManager()->GetAnswerDatabase();
        
        std::vector<ExerciseResult> all_results = saved_data->GetArrayRef();
        const auto& session_results = session_data->GetArrayRef();
        all_results.insert(all_results.end(), session_results.cbegin(), session_results.cend());
        
        return all_results;
    }
    catch (const std::exception&)
    {
        return {};
    }
}

void WindowMain::ShowNoDataMessage() const
{
    wxMessageBox("No statistics data available. Complete some exercises first!", "No Data", wxOK | wxICON_INFORMATION);
}

void WindowMain::CreateAndShowStatisticsWindow(const std::vector<NoteStatistic>& treble_stats, const std::vector<NoteStatistic>& bass_stats)
{
    auto* const window = new WindowStatistics(this, treble_stats, bass_stats);
    window->Show();
}

void WindowMain::CreateAndShowDailyStatisticsWindow(const std::vector<DailyStatisticsRow>& rows)
{
    auto* const window = new WindowDailyStats(nullptr, rows, this);
    window->Show();
}

void WindowMain::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_SPACE)
    {
        const wxPoint mouse_pos = wxGetMousePosition();
        const wxPoint screen_to_client_pos = ScreenToClient(mouse_pos);
        wxMouseEvent mouse_event(wxEVT_LEFT_DOWN);
        mouse_event.SetPosition(screen_to_client_pos);
        mouse_event.SetEventObject(this);
        ProcessWindowEvent(mouse_event);
    }
    else if (event.GetKeyCode() == WXK_ESCAPE)
    {
        wxCommandEvent whavever(wxEVT_TOOL);
        OnToolAbortExercise(whavever);
    }
}

void WindowMain::OnStartButtonClicked()
{
    SoundPlayer::PlayExerciseStartSound();
    SetStopExerciseEnabled(true);
    StartExercise();
    UpdateStatusText();
    UpdateDailyPlayStatus();
    Refresh();
}

void WindowMain::OnClose(wxCloseEvent& event)
{
    if (question_manager.HasUnsavedExerciseResults())
    {
        const int response = wxMessageBox(
            "You have unsaved results, do you wish to save them?",
            "Unsaved Results",
            wxYES_NO | wxICON_WARNING,
            this);

        if (response == wxYES)
        {
            wxCommandEvent whatever(wxEVT_TOOL);
            OnToolSaveSession(whatever);
        }
    }
    event.Skip();
}

void WindowMain::UpdateDailyPlayStatus()
{
    const auto all_results = LoadAllExerciseResults();
    std::unordered_set<std::string> played_days;
    played_days.reserve(all_results.size());
    for (const auto& result : all_results)
    {
        const wxDateTime date(result.GetDateTime());
        played_days.insert(date.FormatISODate().ToStdString());
    }

    const std::string today_key = wxDateTime::Now().FormatISODate().ToStdString();
    has_user_played_today = played_days.find(today_key) != played_days.end();

    unsigned count = 0;
    if (has_user_played_today)
    {
        wxDateTime day = wxDateTime::Now();
        while (true)
        {
            const std::string key = day.FormatISODate().ToStdString();
            if (played_days.find(key) == played_days.end())
                break;
            ++count;
            day = day - wxDateSpan(0, 0, 0, 1);
        }
        streak_days_if_practice_today = count;
    }
    else
    {
        wxDateTime day = wxDateTime::Now() - wxDateSpan(0, 0, 0, 1);
        while (true)
        {
            const std::string key = day.FormatISODate().ToStdString();
            if (played_days.find(key) == played_days.end())
                break;
            ++count;
            day = day - wxDateSpan(0, 0, 0, 1);
        }
        streak_days_if_practice_today = count + 1;
    }
}

void WindowMain::SetStopExerciseEnabled(bool enabled)
{
    toolbar->EnableTool(stop_exercise_tool_id, enabled);
}

std::optional<ENote> WindowMain::GetNoteAtPoint(wxPoint position) const noexcept
{
    unsigned clicked_note = NOT_CLICKED_ON_NOTE;
    for (unsigned i = 0; i < NOTE_COUNT; ++i)
    {
        if (IsPointInCircle(position.x, position.y, note_positions[i].x, note_positions[i].y, ANSWER_CIRCLE_RADIUS))
        {
            clicked_note = i;
            break;
        }
    }
    if (clicked_note == NOT_CLICKED_ON_NOTE)
        return std::nullopt;
    return static_cast<ENote>(clicked_note);
}
