#pragma once
#include "pch.h"
#include "cute_enums.h"

using SettingsAppliedCallback = std::function<void(unsigned, EClefType, bool, unsigned, unsigned, bool)>;

class WindowSettings : public wxFrame
{
	wxPanel* settings_panel{};
	wxSpinCtrl* questions_count_spin_control{};
	wxRadioButton* treble_clef_radio_button{};
	wxRadioButton* bass_clef_radio_button{};
	wxCheckBox* mixed_clef_mode_checkbox{};
	wxCheckBox* midi_enabled_checkbox{};
	wxSpinCtrl* clef_switch_min_notes_spin_control{};
	wxSpinCtrl* clef_switch_max_notes_spin_control{};
	wxButton* apply_settings_button{};
	wxButton* close_settings_button{};
	EClefType selected_clef_type{ EClefType::Treble };
	SettingsAppliedCallback settings_applied_callback;
public:
	explicit WindowSettings(wxWindow* parent, SettingsAppliedCallback callback);
	[[nodiscard]] auto GetQuestionsCount() const noexcept { return unsigned(questions_count_spin_control->GetValue()); }
	[[nodiscard]] auto GetClefType() const noexcept { return selected_clef_type; }
	[[nodiscard]] auto GetIsMixedClefEnabled() const noexcept { return mixed_clef_mode_checkbox->GetValue(); }
	[[nodiscard]] auto GetIsMidiEnabled() const noexcept { return midi_enabled_checkbox->GetValue(); }
	[[nodiscard]] auto GetClefSwitchMinNotes() const noexcept { return unsigned(clef_switch_min_notes_spin_control->GetValue()); }
	[[nodiscard]] auto GetClefSwitchMaxNotes() const noexcept { return unsigned(clef_switch_max_notes_spin_control->GetValue()); }
private:
	void OnRadioButtonSelected(wxCommandEvent&);
	void OnApplyClicked(wxCommandEvent&);
	void OnCloseButtonClicked(wxCommandEvent&);
	void OnClose(wxCloseEvent&);
	void OnMixedClefToggled(wxCommandEvent&);
	void UpdateControlEnablement();

	wxDECLARE_EVENT_TABLE();
};
