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
// NOTE - we are not supporting repeating notes anymore - the event is disabled to prevent missclicks
wxEND_EVENT_TABLE()


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

    stop_excercise_bitmap = wxArtProvider::GetBitmap(wxART_STOP, wxART_MENU);
    SetupMenuBar();
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
    SetStatusText("~excercises~info~", STATUSBAR_ID_EXCERCISES);
}

void WindowMain::ApplySettings(unsigned questions_count, EClefType clef, bool is_mixed_clef, unsigned clef_switch_min_notes, unsigned clef_switch_max_notes, bool is_midi_on)
{
    configured_questions_count = questions_count;
    selected_clef = clef;
    is_mixed_clef_enabled = is_mixed_clef;
    configured_clef_switch_min_notes = clef_switch_min_notes;
    configured_clef_switch_max_notes = clef_switch_max_notes;
    is_midi_enabled = is_midi_on;
    Refresh(false);
}

void WindowMain::SetupMenuBar()
{
	auto menu_bar = new wxMenuBar();
    SetMenuBar(menu_bar);

    /* 1. Menu bar - Actions */
    auto actions_menu = new wxMenu();
    menu_bar->Append(actions_menu, "Actions");

    const wxBitmap forgetIcon = wxArtProvider::GetBitmap(wxART_UNDO, wxART_MENU);
    const wxBitmap saveIcon = wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU);
    const wxBitmap exitIcon = wxArtProvider::GetBitmap(wxART_QUIT, wxART_MENU);
    const wxBitmap settingsIcon = wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_MENU);

    stop_excercise_menu_item = actions_menu->Append(wxID_ANY, "Stop excercise");
    DisableStopExcercise();
    actions_menu->AppendSeparator();
    auto save_menu_item = actions_menu->Append(wxID_ANY, "Save session");
    save_menu_item->SetBitmap(saveIcon);
    auto forget_menu_item = actions_menu->Append(wxID_ANY, "Forget session");
    forget_menu_item->SetBitmap(forgetIcon);
    actions_menu->AppendSeparator();
    auto settings_menu_item = actions_menu->Append(wxID_ANY, "Settings...");
    settings_menu_item->SetBitmap(settingsIcon);
    actions_menu->AppendSeparator();
    auto exit_menu_item = actions_menu->Append(wxID_EXIT, "Exit");
    exit_menu_item->SetBitmap(exitIcon);

    settings_menu_item->Enable(true);

    Bind(wxEVT_MENU, &WindowMain::OnMenuAbortExcercise, this, stop_excercise_menu_item->GetId());
	Bind(wxEVT_MENU, &WindowMain::OnMenuSettings, this, settings_menu_item->GetId());
	Bind(wxEVT_MENU, &WindowMain::OnMenuExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &WindowMain::OnMenuSaveSession, this, save_menu_item->GetId());
    Bind(wxEVT_MENU, &WindowMain::OnMenuForgetSession, this, forget_menu_item->GetId());

    /* 2. Menu bar - Data */
    auto data_menu = new wxMenu();
    menu_bar->Append(data_menu, "Data");

    auto all_stats_menu_item = data_menu->Append(wxID_ANY, "Show All Stats");
    Bind(wxEVT_MENU, &WindowMain::OnMenuShowAllStats, this, all_stats_menu_item->GetId());
    auto motivation_menu_item = data_menu->Append(wxID_ANY, "Show Worst Notes");
    Bind(wxEVT_MENU, &WindowMain::OnMenuShowMotivation, this, motivation_menu_item->GetId());
    auto daily_stats_menu_item = data_menu->Append(wxID_ANY, "Show Daily Stats");
    Bind(wxEVT_MENU, &WindowMain::OnMenuShowDailyStats, this, daily_stats_menu_item->GetId());
}

void WindowMain::StartExcercise()
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

void WindowMain::FinishExcercise()
{
    is_started = false;
    is_last_answered_note_correct = NoteFeedback::None;
    DisableStopExcercise();

    const auto average_ms = question_manager.GetAverageAnswerTime();
    const auto average_seconds = float(average_ms.count()) / MILLISECONDS_TO_SECONDS_FACTOR;
    const auto msg = std::format("Average time: {:.02f}", average_seconds);

    wxMessageBox(msg, "Result", wxOK | wxICON_QUESTION, this);
    UpdateDailyPlayStatus();
}

void WindowMain::UpdateStatusText()
{
    const auto [questions_status, answers_status, excercises_status] = question_manager.GetStatusText();
    SetStatusText(answers_status, STATUSBAR_ID_ANSWERS);
    SetStatusText(questions_status, STATUSBAR_ID_QUESTIONS);
    SetStatusText(excercises_status, STATUSBAR_ID_EXCERCISES);
}

void WindowMain::OnPaint(wxPaintEvent&)
{
    wxPaintDC drawing_context(this);
    const auto hovered_from_motivation = WindowMotivation::GetHoveredNoteIfAny();
    const std::optional<ESpecificNote> external_hovered_specific_note = hovered_from_motivation.has_value() ? std::optional<ESpecificNote>(hovered_from_motivation->GetSpecificNote()) : std::nullopt;
    const std::optional<EClefType> external_hovered_clef = hovered_from_motivation.has_value() ? std::optional<EClefType>(hovered_from_motivation->GetClef()) : std::nullopt;
    const RenderingState rendering_state{
        .is_excercise_started = is_started,
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
        is_last_answered_note_correct = NoteFeedback::Correct;
        if (specific_note.has_value() && is_midi_enabled)
        {
            MidiPlayer::Play(specific_note.value());
        }
        else
        {
            //SHULD NOT BE POSSIBLE
            //std::unreachable()
        }
    }
    else
    {
        SoundPlayer::PlayNoteClickSound();
        is_last_answered_note_correct = NoteFeedback::Wrong;
    }
    last_answered_clef = current_clef;
    last_answered_note = specific_note.value_or(ESpecificNote::NOTE_C_0);

    UpdateStatusText();
    Refresh();
    if (question_manager.IsEnd())
        FinishExcercise();
}

void WindowMain::OnMouseRightClick(wxMouseEvent&)
{
    wxCommandEvent whatever(wxEVT_MENU);
    OnMenuAbortExcercise(whatever);
}

void WindowMain::OnMouseMove(wxMouseEvent& event)
{
    hovered_note = GetNoteAtPoint(event.GetPosition());

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

void WindowMain::OnMenuSettings(wxCommandEvent&)
{
    settings_window->Show();
    settings_window->Raise();
}

void WindowMain::OnMenuExit(wxCommandEvent&)
{
    Close();
}

void WindowMain::OnMenuAbortExcercise(wxCommandEvent&)
{
    if (!is_started)
        return;
    SoundPlayer::PlayAbandonSound();
    is_started = false;
    is_last_answered_note_correct = NoteFeedback::None;
    question_manager.AbortExcercise();
    UpdateStatusText();
    UpdateDailyPlayStatus();
    DisableStopExcercise();
    Refresh();
}

void WindowMain::OnMenuSaveSession(wxCommandEvent&)
{
    const EFileSaveResult result = question_manager.SaveExcerciseStats();
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
    default:
        wxMessageBox("OnMenuSaveSession: file save result unknown", "Internal Error", wxOK | wxICON_ERROR);
        break;
    }
    UpdateStatusText();
    UpdateDailyPlayStatus();
    Refresh();
}

void WindowMain::OnMenuForgetSession(wxCommandEvent&)
{
    const auto excerciseCount = question_manager.GetAnswerManager()->GetAnswerDatabase()->GetExcerciseCount();
    if (excerciseCount == 0)
    {
        return;
    }
    
    const int res = wxMessageBox(std::format("Forget {} excercise results?", excerciseCount), "Confirm", wxYES_NO | wxICON_QUESTION, this);
    if (res != wxYES)
    {
        return;
    }

    question_manager.ResetAllSessionAnswers();
    UpdateStatusText();
    UpdateDailyPlayStatus();
    Refresh();
}

void WindowMain::OnMenuShowAllStats(wxCommandEvent&)
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

void WindowMain::OnMenuShowMotivation(wxCommandEvent&)
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

void WindowMain::OnMenuShowDailyStats(wxCommandEvent&)
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
        wxCommandEvent whavever(wxEVT_MENU);
        OnMenuAbortExcercise(whavever);
    }
}

void WindowMain::OnStartButtonClicked()
{
    SoundPlayer::PlayExcerciseStartSound();
    EnableStopExcercise();
    StartExcercise();
    UpdateStatusText();
    UpdateDailyPlayStatus();
    Refresh();
}

void WindowMain::OnClose(wxCloseEvent& event)
{
    if (question_manager.HasUnsavedExcerciseResults())
    {
        const int response = wxMessageBox(
            "You have unsaved results, do you wish to save them?",
            "Unsaved Results",
            wxYES_NO | wxICON_WARNING,
            this);

        if (response == wxYES)
        {
            wxCommandEvent whatever(wxEVT_MENU);
            OnMenuSaveSession(whatever);
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

void WindowMain::EnableStopExcercise()
{
    stop_excercise_menu_item->SetBitmap(stop_excercise_bitmap);
    stop_excercise_menu_item->Enable(true);
}

void WindowMain::DisableStopExcercise()
{
    stop_excercise_menu_item->SetBitmap(wxNullBitmap);
    stop_excercise_menu_item->Enable(false);
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
