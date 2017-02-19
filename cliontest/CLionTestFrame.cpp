//
// Created by Arnold Müller on 11.02.17.
//

#include "CLionTestFrame.h"

CLionTestFrame::CLionTestFrame() : wxFrame(NULL, wxID_ANY, "Hallo"), tim_(this) {
    CreateStatusBar();
    SetSize(900, 600);
    Bind(wxEVT_PAINT, &CLionTestFrame::OnPaint, this, wxID_ANY);
    Bind(wxEVT_TIMER, &CLionTestFrame::OnTimer, this, wxID_ANY);
    prop_.AddReaderWriter(*this);
    prop_.AddReaderWriter(universe);
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
    prop_.UpdateReaders();
    universe.Initialize();
    Start();
}
