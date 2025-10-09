#pragma once
#include "pch.h"
#include "Note.h"

struct NoteStat;

class WindowMotivation final : public wxFrame
{
    wxButton* suppress_for_today_button{};
    wxStaticText* currently_hovered_label_control{};
    std::unordered_map<wxStaticText*, Note> label_control_to_note_map;
    Note currently_hovered_note{};

    static inline WindowMotivation* active_window_instance = nullptr;
public:
    explicit WindowMotivation(wxWindow* parent, size_t worst_notes_count = 2, bool is_suppression_button_visible = true, bool should_stay_on_top = true);
    ~WindowMotivation() override;
    [[nodiscard]] Note GetCurrentlyHoveredNote() const { return currently_hovered_note; }
    [[nodiscard]] bool HasHoveredNote() const;

    static [[nodiscard]] bool ShouldShowToday();
    static [[nodiscard]] bool IsOpen();
    static [[nodiscard]] std::optional<Note> GetHoveredNoteIfAny();
    static void MarkDismissedToday();
    static void RaiseIfOpen();
private:
    [[nodiscard]] wxStaticBoxSizer* CreateNotesListStaticBoxSizer(wxWindow* parent, const wxString& box_label, const std::vector<NoteStat>& note_stats, EClefType clef_type);
    void BuildContent(size_t worst_notes_count, bool is_suppression_button_visible);
    void OnNoteLabelEnter(wxMouseEvent&);
    void OnNoteLabelLeave(wxMouseEvent&);
    void OnAnyMouseDown(wxMouseEvent&);
    void OnKeyHook(wxKeyEvent&);
    void OnActivate(wxActivateEvent&);
    void OnActivateApp(wxActivateEvent&);
    void ApplyHoveredStyleToLabel(wxStaticText* label);
    void ApplyNormalStyleToLabel(wxStaticText* label);
    void OnSuppressForTodayButtonClicked(wxCommandEvent&);
};
