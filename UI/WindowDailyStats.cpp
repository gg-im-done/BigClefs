#include "pch.h"
#include "UI/WindowDailyStats.h"

namespace
{
    using ColumnWidthArray = std::array<int, 10>;

    constexpr ColumnWidthArray DESIRED_COLUMN_WIDTHS{ 80, 60, 60, 60, 60, 60, 60, 80, 80, 90 };
    constexpr ColumnWidthArray MINIMUM_COLUMN_WIDTHS{ 60, 50, 50, 50, 50, 50, 50, 60, 60, 70 };
    constexpr int DESIRED_TOTAL_WIDTH = std::accumulate(DESIRED_COLUMN_WIDTHS.cbegin(), DESIRED_COLUMN_WIDTHS.cend(), 0);
    constexpr int MINIMUM_TOTAL_WIDTH = std::accumulate(MINIMUM_COLUMN_WIDTHS.cbegin(), MINIMUM_COLUMN_WIDTHS.cend(), 0);

    constexpr int BORDER_PADDING = 6;
}

WindowDailyStats::WindowDailyStats(wxWindow* parent, const std::vector<DailyStatisticsRow>& rows, const wxWindow* main_window)
    : wxFrame(parent, wxID_ANY, "", wxDefaultPosition, wxSize(900, 1200), wxFRAME_NO_TASKBAR | wxBORDER_NONE),
      rows(rows)
{
    SetBackgroundColour(*wxWHITE);
    BuildContent();
    Bind(wxEVT_ACTIVATE, &WindowDailyStats::OnActivate, this);
    Bind(wxEVT_ACTIVATE_APP, &WindowDailyStats::OnActivateApp, this);
    Bind(wxEVT_SIZE, &WindowDailyStats::OnSize, this);
    SetMinSize(wxSize(800, 900));
    const wxSize size = main_window->GetSize();
    const auto target_window_height = static_cast<int>(size.GetHeight() * 0.8);
    const auto target_window_width = static_cast<int>(size.GetWidth() * 0.8);
    SetSize(target_window_width, target_window_height);
    CentreOnScreen();
}

void WindowDailyStats::BuildContent()
{
    content_panel = new wxPanel(this);

    auto* const root_sizer = new wxBoxSizer(wxVERTICAL);

    auto* const title_text = new wxStaticText(content_panel, wxID_ANY, "Daily Stats");
    auto title_font = title_text->GetFont();
    title_font.MakeBold();
    title_font.Scale(1.1f);
    title_text->SetFont(title_font);

    daily_list = new wxListCtrl(content_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    daily_list->AppendColumn("Date", wxLIST_FORMAT_LEFT, DESIRED_COLUMN_WIDTHS[0]);
    daily_list->AppendColumn("Treble", wxLIST_FORMAT_RIGHT, DESIRED_COLUMN_WIDTHS[1]);
    daily_list->AppendColumn("Bass", wxLIST_FORMAT_RIGHT, DESIRED_COLUMN_WIDTHS[2]);
    daily_list->AppendColumn("Treble %", wxLIST_FORMAT_RIGHT, DESIRED_COLUMN_WIDTHS[3]);
    daily_list->AppendColumn("Bass %", wxLIST_FORMAT_RIGHT, DESIRED_COLUMN_WIDTHS[4]);
    daily_list->AppendColumn("Total", wxLIST_FORMAT_RIGHT, DESIRED_COLUMN_WIDTHS[5]);
    daily_list->AppendColumn("Total %", wxLIST_FORMAT_RIGHT, DESIRED_COLUMN_WIDTHS[6]);
    daily_list->AppendColumn("Treble Time", wxLIST_FORMAT_RIGHT, DESIRED_COLUMN_WIDTHS[7]);
    daily_list->AppendColumn("Bass Time", wxLIST_FORMAT_RIGHT, DESIRED_COLUMN_WIDTHS[8]);
    daily_list->AppendColumn("Average Time", wxLIST_FORMAT_RIGHT, DESIRED_COLUMN_WIDTHS[9]);

    root_sizer->Add(title_text, 0, wxALIGN_CENTER | wxALL, 10);
    root_sizer->Add(daily_list, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 15);

    content_panel->SetSizer(root_sizer);

    Populate();
    AdjustColumnWidthsToFit();
    Layout();
}

void WindowDailyStats::Populate()
{
    for (long row_index = 0; const auto& row : rows)
    {
        daily_list->InsertItem(row_index, row.date_label);
        daily_list->SetItem(row_index, 1, row.treble_total  == 0 ? "-" : std::format("{}", row.treble_total));
        daily_list->SetItem(row_index, 2, row.bass_total    == 0 ? "-" : std::format("{}", row.bass_total));
        daily_list->SetItem(row_index, 3, row.treble_total  == 0 ? "-" : std::format("{:.1f}%", row.treble_accuracy_percent));
        daily_list->SetItem(row_index, 4, row.bass_total    == 0 ? "-" : std::format("{:.1f}%", row.bass_accuracy_percent));
        daily_list->SetItem(row_index, 5, row.total_answers == 0 ? "-" : std::format("{}", row.total_answers));
        daily_list->SetItem(row_index, 6, row.total_answers == 0 ? "-" : std::format("{:.1f}%", row.overall_accuracy_percent));
        daily_list->SetItem(row_index, 7, row.treble_total  == 0 ? "-" : std::format("{}ms", row.treble_average_time.count()));
        daily_list->SetItem(row_index, 8, row.bass_total    == 0 ? "-" : std::format("{}ms", row.bass_average_time.count()));
        daily_list->SetItem(row_index, 9, row.total_answers == 0 ? "-" : std::format("{}ms", row.overall_average_time.count()));
        ++row_index;
    }
}

void WindowDailyStats::OnActivate(wxActivateEvent& event)
{
    if (!event.GetActive())
    {
        Destroy();
        return;
    }
    event.Skip();
}

void WindowDailyStats::OnActivateApp(wxActivateEvent& event)
{
    if (!event.GetActive())
    {
        Destroy();
        return;
    }
    event.Skip();
}

void WindowDailyStats::OnSize(wxSizeEvent& event)
{
    Layout();
    AdjustColumnWidthsToFit();
    event.Skip();
}

void WindowDailyStats::AdjustColumnWidthsToFit()
{
    const int list_client_width = daily_list->GetClientSize().GetWidth();
    if (list_client_width <= 0)
    {
        return;
    }
    const int vertical_scrollbar_width = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, this);
    const int available_width = list_client_width - vertical_scrollbar_width - BORDER_PADDING;
    if (available_width <= 0)
    {
        return;
    }

    ColumnWidthArray final_column_widths{};
    if (available_width >= DESIRED_TOTAL_WIDTH)
    {
        final_column_widths = DESIRED_COLUMN_WIDTHS;
    }
    else
    {
        const double interpolation_scale = std::max(0.0, static_cast<double>(available_width - MINIMUM_TOTAL_WIDTH) / std::max(1.0, static_cast<double>(DESIRED_TOTAL_WIDTH - MINIMUM_TOTAL_WIDTH)));
        int allocated_width = 0;
        for (size_t i = 0; i < final_column_widths.size(); ++i)
        {
            const int additional_width = static_cast<int>(std::floor((DESIRED_COLUMN_WIDTHS[i] - MINIMUM_COLUMN_WIDTHS[i]) * interpolation_scale));
            final_column_widths[i] = MINIMUM_COLUMN_WIDTHS[i] + additional_width;
            allocated_width += final_column_widths[i];
        }
    }
    for (size_t i = 0; i < final_column_widths.size(); ++i)
    {
        daily_list->SetColumnWidth(static_cast<int>(i), std::max(MINIMUM_COLUMN_WIDTHS[i], final_column_widths[i]));
    }
}
