#pragma once
#include "pch.h"
#include "DailyStatistics.h"

class WindowDailyStats : public wxFrame
{
    wxPanel* content_panel{};
    wxListCtrl* daily_list{};
    std::vector<DailyStatisticsRow> rows;
public:
    explicit WindowDailyStats(wxWindow* parent, const std::vector<DailyStatisticsRow>& rows, const wxWindow* main_window);
private:
    void BuildContent();
    void Populate();
    void OnActivate(wxActivateEvent&);
    void OnActivateApp(wxActivateEvent&);
    void OnSize(wxSizeEvent&);
    void AdjustColumnWidthsToFit();
};
