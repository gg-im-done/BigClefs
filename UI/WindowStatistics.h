#pragma once
#include "pch.h"
#include "NoteStatistics.h"

class WindowStatistics : public wxFrame
{
	wxPanel* panel{};
	wxListCtrl* treble_list{};
	wxListCtrl* bass_list{};
	wxButton* close_button{};
	
	std::vector<NoteStatistic> treble_stats;
	std::vector<NoteStatistic> bass_stats;

public:
	explicit WindowStatistics(wxWindow* parent, 
		const std::vector<NoteStatistic>& treble_stats,
		const std::vector<NoteStatistic>& bass_stats);

private:
	void SetupLayout();
	void CreateTrebleStatisticsTable(wxBoxSizer* sizer);
	void CreateBassStatisticsTable(wxBoxSizer* sizer);
	void CreateStatisticsTable(wxListCtrl*& list, wxBoxSizer* sizer, const std::string& title);
	void PopulateList(wxListCtrl* list, const std::vector<NoteStatistic>& stats);
	void OnCloseButtonClicked(wxCommandEvent&);
	void OnClose(wxCloseEvent&);

	wxDECLARE_EVENT_TABLE();
};
