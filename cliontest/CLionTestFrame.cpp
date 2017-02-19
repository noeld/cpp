//
// Created by Arnold Müller on 11.02.17.
//

#include "CLionTestFrame.h"

CLionTestFrame::CLionTestFrame() : wxFrame(NULL, wxID_ANY, "Hallo"), tim_(this) {
    CreateStatusBar();

    wxMenu* menuA = new wxMenu;
    menuA->Append(ID_Menu_Restart, "&Restart", "Restart univers with new, random setup");
    menuA->Append(ID_Menu_Stop, "&Pause", "Pause execution");
    menuA->Append(ID_Menu_Start, "&Continue", "Continue execution");
    menuA->AppendSeparator();
    menuA->Append(ID_Menu_Reload, "Re&load", "Reload configuration");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuA, "&File");
    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &CLionTestFrame::OnRestart, this, ID_Menu_Restart);
    Bind(wxEVT_MENU, &CLionTestFrame::OnStop, this, ID_Menu_Stop);
    Bind(wxEVT_MENU, &CLionTestFrame::OnStart, this, ID_Menu_Start);
    Bind(wxEVT_MENU, &CLionTestFrame::OnReload, this, ID_Menu_Reload);

    SetSize(900, 600);
    Bind(wxEVT_PAINT, &CLionTestFrame::OnPaint, this, wxID_ANY);
    Bind(wxEVT_TIMER, &CLionTestFrame::OnTimer, this, wxID_ANY);
    prop_.AddReaderWriter(*this);
    prop_.AddReaderWriter(universe);
    prop_.generate_pos_xmax = GetClientSize().x;
    prop_.generate_pos_ymax = GetClientSize().y;
    Restart();
}

void CLionTestFrame::OnPaint(wxPaintEvent &event) {
    wxPaintDC dc(this);
    //auto s = GetClientSize();
    //dc.DrawText("Hallo", s.GetWidth()/2, s.GetHeight()/2);
    for (auto& o : universe.getObjects()) {
        if (!o.isActive())
            continue;
        dc.SetBrush(wxBrush(wxColour(o.getRgb_())));
        dc.DrawCircle(o.getPos().getX(), o.getPos().getY(), o.getR());
    }
}

void CLionTestFrame::OnTimer(wxTimerEvent &event) {
    universe.advance(simulation_f * simulation_intervall / 1000.0 );
    ++c_;
    Refresh();
    if  (c_ % 25 == 0)
        SetStatusText(std::to_string(universe.getNr_()));

}

void CLionTestFrame::ReadProperties(const Properties &properties) {
    simulation_intervall = properties.simulation_intervall;
    simulation_f = properties.simulation_f;
}

void CLionTestFrame::WriteProperties(Properties &properties) {
    properties.simulation_intervall = simulation_intervall;
    properties.simulation_f = simulation_f;
    properties.generate_pos_xmax = GetClientSize().x;
    properties.generate_pos_ymax = GetClientSize().y;
}

CLionTestFrame::~CLionTestFrame() {
    prop_.UpdateWriters();
    prop_.RemoveReaderWriter(universe);
    prop_.RemoveReaderWriter(*this);
}

void CLionTestFrame::Start() {
    prop_.UpdateReaders();
    tim_.Start(this->simulation_intervall);
    SetStatusText("Simulation started");
}

void CLionTestFrame::Stop() {
    tim_.Stop();
}

void CLionTestFrame::Restart() {
    Stop();
    prop_.generate_pos_xmax = GetClientSize().x;
    prop_.generate_pos_ymax = GetClientSize().y;
    prop_.UpdateReaders();
    universe.Initialize();
    Start();
}

void CLionTestFrame::OnRestart(wxCommandEvent &event) {
    Restart();
}
void CLionTestFrame::OnStop(wxCommandEvent &event) {
    Stop();
}
void CLionTestFrame::OnStart(wxCommandEvent &event) {
    Start();
}

void CLionTestFrame::OnReload(wxCommandEvent &event) {
    prop_.ReadConfigFile(Properties::filename_, prop_.configname_);
    prop_.UpdateReaders();
}
