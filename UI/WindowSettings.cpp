#include "pch.h"
#include "UI/WindowSettings.h"
#include "app_constants.h"

wxBEGIN_EVENT_TABLE(WindowSettings, wxFrame)
wxEND_EVENT_TABLE()

WindowSettings::WindowSettings(wxWindow* parent, SettingsAppliedCallback callback)
    : wxFrame(parent, wxID_ANY, wxT("Settings"), parent->GetPosition(), wxDefaultSize), settings_applied_callback(std::move(callback))
{
    settings_panel = new wxPanel(this, wxID_ANY);

    auto settings_font = settings_panel->GetFont();
    const auto settings_font_size = settings_font.GetPointSize();
    settings_font.SetPointSize(settings_font_size + 1);
    settings_panel->SetFont(settings_font);

    auto* const settings_window_main_sizer = new wxBoxSizer(wxVERTICAL);
    settings_panel->SetSizer(settings_window_main_sizer);

    auto* const clef_selection_group_box = new wxStaticBox(settings_panel, wxID_ANY, "Clef");
    auto* const clef_selection_sizer = new wxStaticBoxSizer(clef_selection_group_box, wxVERTICAL);
    treble_clef_radio_button = new wxRadioButton(settings_panel, wxID_ANY, "Treble Clef", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    bass_clef_radio_button = new wxRadioButton(settings_panel, wxID_ANY, "Bass Clef");
    clef_selection_sizer->Add(treble_clef_radio_button, 0, wxALL, 8);
    clef_selection_sizer->Add(bass_clef_radio_button, 0, wxALL, 8);
    treble_clef_radio_button->SetValue(true);
    settings_window_main_sizer->Add(clef_selection_sizer, 0, wxEXPAND | wxALL, 14);
    Bind(wxEVT_RADIOBUTTON, &WindowSettings::OnRadioButtonSelected, this);

    auto* const questions_row_sizer = new wxBoxSizer(wxHORIZONTAL);
    auto* const questions_count_label = new wxStaticText(settings_panel, wxID_ANY, "Questions:");
    questions_count_spin_control = new wxSpinCtrl(settings_panel, wxID_ANY, "25", wxDefaultPosition, wxSize(100, -1));
    questions_count_spin_control->SetRange(1, MAX_QUESTIONS_COUNT);
    questions_count_spin_control->SetValue(25);
    questions_row_sizer->Add(questions_count_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    questions_row_sizer->Add(questions_count_spin_control, 0);
    settings_window_main_sizer->Add(questions_row_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 14);

    mixed_clef_mode_checkbox = new wxCheckBox(settings_panel, wxID_ANY, "Enable Mixed Clef");
    mixed_clef_mode_checkbox->SetValue(true);
    settings_window_main_sizer->Add(mixed_clef_mode_checkbox, 0, wxLEFT | wxRIGHT | wxBOTTOM, 14);
    mixed_clef_mode_checkbox->Bind(wxEVT_CHECKBOX, &WindowSettings::OnMixedClefToggled, this);

    midi_enabled_checkbox = new wxCheckBox(settings_panel, wxID_ANY, "Enable MIDI Sound");
    midi_enabled_checkbox->SetValue(DEFAULT_MIDI_ENABLED);
    settings_window_main_sizer->Add(midi_enabled_checkbox, 0, wxLEFT | wxRIGHT | wxBOTTOM, 14);

    auto* const clef_switch_sizer_vertical = new wxBoxSizer(wxVERTICAL);

    auto* const clef_switch_min_row_sizer = new wxBoxSizer(wxHORIZONTAL);
    auto* const clef_switch_min_label = new wxStaticText(settings_panel, wxID_ANY, "Clef switch min (notes):");
    clef_switch_min_notes_spin_control = new wxSpinCtrl(
        settings_panel,
        wxID_ANY,
        wxString::Format("%u", DEFAULT_CLEF_SWITCH_MIN_NOTES),
        wxDefaultPosition,
        wxSize(100, -1)
    );
    clef_switch_min_notes_spin_control->SetRange(1, MAX_CLEF_SWITCH_NOTES);
    clef_switch_min_notes_spin_control->SetValue(DEFAULT_CLEF_SWITCH_MIN_NOTES);
    clef_switch_min_row_sizer->Add(clef_switch_min_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    clef_switch_min_row_sizer->Add(clef_switch_min_notes_spin_control, 0);

    auto* const clef_switch_max_row_sizer = new wxBoxSizer(wxHORIZONTAL);
    auto* const clef_switch_max_label = new wxStaticText(settings_panel, wxID_ANY, "Clef switch max (notes):");
    clef_switch_max_notes_spin_control = new wxSpinCtrl(
        settings_panel,
        wxID_ANY,
        wxString::Format("%u", DEFAULT_CLEF_SWITCH_MAX_NOTES),
        wxDefaultPosition,
        wxSize(100, -1)
    );
    clef_switch_max_notes_spin_control->SetRange(1, MAX_CLEF_SWITCH_NOTES);
    clef_switch_max_notes_spin_control->SetValue(DEFAULT_CLEF_SWITCH_MAX_NOTES);
    clef_switch_max_row_sizer->Add(clef_switch_max_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    clef_switch_max_row_sizer->Add(clef_switch_max_notes_spin_control, 0);

    clef_switch_sizer_vertical->Add(clef_switch_min_row_sizer, 0, wxBOTTOM, 8);
    clef_switch_sizer_vertical->Add(clef_switch_max_row_sizer, 0);
    settings_window_main_sizer->Add(clef_switch_sizer_vertical, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 14);

    settings_window_main_sizer->AddStretchSpacer(1);

    apply_settings_button = new wxButton(settings_panel, wxID_APPLY, "Apply");
    close_settings_button = new wxButton(settings_panel, wxID_CLOSE, "Close");
    auto* const standard_dialog_button_sizer = new wxStdDialogButtonSizer();
    standard_dialog_button_sizer->AddButton(apply_settings_button);
    standard_dialog_button_sizer->AddStretchSpacer();
    standard_dialog_button_sizer->AddButton(close_settings_button);
    standard_dialog_button_sizer->Realize();
    settings_window_main_sizer->Add(standard_dialog_button_sizer, 0, wxALIGN_CENTER | wxALL, 14);

    apply_settings_button->Bind(wxEVT_BUTTON, &WindowSettings::OnApplyClicked, this);
    close_settings_button->Bind(wxEVT_BUTTON, &WindowSettings::OnCloseButtonClicked, this);
    Bind(wxEVT_CLOSE_WINDOW, &WindowSettings::OnClose, this);

    UpdateControlEnablement();
    settings_window_main_sizer->Fit(this);
    settings_window_main_sizer->SetSizeHints(this);
    this->CenterOnParent();

    apply_settings_button->SetFocus();
}

void WindowSettings::OnApplyClicked(wxCommandEvent&)
{
    if (settings_applied_callback)
    {
        settings_applied_callback(
            GetQuestionsCount(),
            GetClefType(),
            GetIsMixedClefEnabled(),
            GetClefSwitchMinNotes(),
            GetClefSwitchMaxNotes(),
            GetIsMidiEnabled()
        );
    }
    Hide();
}

void WindowSettings::OnCloseButtonClicked(wxCommandEvent&)
{
    Hide();
}

void WindowSettings::OnRadioButtonSelected(wxCommandEvent& event)
{
    static const int TREBLE_CLEF_ID = treble_clef_radio_button->GetId();
    static const int BASS_CLEF_ID = bass_clef_radio_button->GetId();
    const auto id = event.GetId();

    if (id == TREBLE_CLEF_ID)
    {
        selected_clef_type = EClefType::Treble;
    }
    else if (id == BASS_CLEF_ID)
    {
        selected_clef_type = EClefType::Bass;
    }
    else
    {
        wxMessageBox("Unknown clef type selected");
        selected_clef_type = EClefType::Treble;
    }
}

void WindowSettings::OnClose(wxCloseEvent& event)
{
	Hide();
	event.Veto(); // prevent window from being destroyed
}

void WindowSettings::OnMixedClefToggled(wxCommandEvent&)
{
    UpdateControlEnablement();
}

void WindowSettings::UpdateControlEnablement()
{
    const bool is_mixed = mixed_clef_mode_checkbox->GetValue();
    treble_clef_radio_button->Enable(!is_mixed);
    bass_clef_radio_button->Enable(!is_mixed);
    clef_switch_min_notes_spin_control->Enable(is_mixed);
    clef_switch_max_notes_spin_control->Enable(is_mixed);
}
