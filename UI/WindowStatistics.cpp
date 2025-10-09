#include "pch.h"
#include "WindowStatistics.h"
#include "util.h"

enum
{
	ID_CLOSE_BUTTON = 1000
};

wxBEGIN_EVENT_TABLE(WindowStatistics, wxFrame)
	EVT_BUTTON(ID_CLOSE_BUTTON, WindowStatistics::OnCloseButtonClicked)
	EVT_CLOSE(WindowStatistics::OnClose)
wxEND_EVENT_TABLE()

WindowStatistics::WindowStatistics(wxWindow* parent, 
	const std::vector<NoteStatistic>& treble_stats,
	const std::vector<NoteStatistic>& bass_stats)
	: wxFrame(parent, wxID_ANY, "Note Statistics", wxDefaultPosition, wxSize(800, 600)),
	treble_stats(treble_stats), bass_stats(bass_stats)
{
	SetupLayout();
	PopulateList(treble_list, treble_stats);
	PopulateList(bass_list, bass_stats);

	CenterOnParent();
}

void WindowStatistics::SetupLayout()
{
	panel = new wxPanel(this);
	
	auto* const main_sizer = new wxBoxSizer(wxVERTICAL);
	auto* const tables_sizer = new wxBoxSizer(wxHORIZONTAL);
	
	CreateTrebleStatisticsTable(tables_sizer);
	CreateBassStatisticsTable(tables_sizer);
	
	close_button = new wxButton(panel, ID_CLOSE_BUTTON, "Close");
	
	main_sizer->Add(tables_sizer, 1, wxEXPAND);
	main_sizer->Add(close_button, 0, wxALIGN_CENTER | wxALL, 10);
	
	panel->SetSizer(main_sizer);
}

void WindowStatistics::PopulateList(wxListCtrl* list, const std::vector<NoteStatistic>& stats)
{
	long row = 0;
	for (const auto& stat : stats)
	{
		const std::string note_name = NoteToStringMinimal(stat.note);
		const std::string time = std::format("{}ms", stat.average_time.count());
		const std::string attempts = std::format("{}", stat.attempts);
		const std::string accuracy = std::format("{:.1f}%", stat.GetAccuracyPercentage());
		
		list->InsertItem(row, note_name);
		list->SetItem(row, 1, time);
		list->SetItem(row, 2, attempts);
		list->SetItem(row, 3, accuracy);
		
		++row;
	}
}

void WindowStatistics::OnCloseButtonClicked(wxCommandEvent&)
{
	Close();
}

void WindowStatistics::CreateTrebleStatisticsTable(wxBoxSizer* sizer)
{
	CreateStatisticsTable(treble_list, sizer, "Treble Clef");
}

void WindowStatistics::CreateBassStatisticsTable(wxBoxSizer* sizer)
{
	CreateStatisticsTable(bass_list, sizer, "Bass Clef");
}

void WindowStatistics::CreateStatisticsTable(wxListCtrl*& list, wxBoxSizer* sizer, const std::string& title)
{
	auto* const table_panel = new wxPanel(panel);
	auto* const table_sizer = new wxBoxSizer(wxVERTICAL);
	
	auto* const title_text = new wxStaticText(table_panel, wxID_ANY, title);
	list = new wxListCtrl(table_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	
	list->AppendColumn("Note", wxLIST_FORMAT_LEFT, 80);
	list->AppendColumn("Avg Time", wxLIST_FORMAT_RIGHT, 80);
	list->AppendColumn("Attempts", wxLIST_FORMAT_RIGHT, 80);
	list->AppendColumn("Accuracy", wxLIST_FORMAT_RIGHT, 80);
	
	table_sizer->Add(title_text, 0, wxALIGN_CENTER | wxALL, 5);
	table_sizer->Add(list, 1, wxEXPAND | wxALL, 5);
	
	table_panel->SetSizer(table_sizer);
	sizer->Add(table_panel, 1, wxEXPAND | wxALL, 10);
}

void WindowStatistics::OnClose(wxCloseEvent&)
{
	Destroy();
}
