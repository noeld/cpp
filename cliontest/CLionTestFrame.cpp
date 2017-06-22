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
    prop_.AddReaderWriter(PlanetPropertyReaderWriter::instance());
    prop_.generate_pos_xmax = GetClientSize().x;
    prop_.generate_pos_ymax = GetClientSize().y;

    this->SetBackgroundColour(wxColour(0x33, 0x33, 0x33));
    Restart();
}

void CLionTestFrame::OnPaint(wxPaintEvent &event) {
    wxPaintDC dc(this);
    //auto s = GetClientSize();
    //dc.DrawText("Hallo", s.GetWidth()/2, s.GetHeight()/2);
    dc.DrawLine(5, 102, 4 + 100, 102);
    unsigned hx = 0;
    for (const auto& h : hist_) {
        dc.DrawLine(5 + hx, 100, 5 + hx, 100 - std::get<3>(h));
        ++hx;
    }
    //for (auto& o : universe.getObjects()) {
    for(size_t i = 0; i < universe.getActiveObjects(); ++i) {
        //if (!o.isActive())
          //  continue;
        auto& o = universe.getObjects()[i];
        dc.SetBrush(wxBrush(wxColour(o.getRgb_())));
        //dc.DrawPoint(o.getPos().getX(), o.getPos().getY());
        dc.DrawCircle(o.getPos().getX(), o.getPos().getY(), o.getR());
    }
}
template<class T>
struct MassAccessor {
    double operator()(const T& e) const { return e.getMass(); }
};
void CLionTestFrame::OnTimer(wxTimerEvent &event) {
    static double lastt = 0;
    static double lastc = 0;
    double t = tm_.Duration().count() / 1'000'000'000.0;
    universe.advance( t * simulation_f );
    if  (c_ % 25 == 0) {
        double delta_t = t - lastt;
        double delta_c = c_ - lastc;
        double framerate = delta_c / delta_t;
        lastt = t;
        lastc = c_;
        SetStatusText(std::to_string(universe.getNr_()) + "; FR: " + std::to_string(delta_t));
        //Histogram<decltype(universe.getObjects()), [=]( decltype(universe.getObjects().begin())& e) { return e->getMass(); }> h(200, 100);
        //Histogram<std::remove_reference<decltype(universe.getObjects())>::type, double(decltype(universe.getObjects().begin())&)> h(200, 100);
        //Histogram<std::remove_reference<decltype(universe.getObjects())>::type, MassAccessor<std::remove_reference<decltype(universe.getObjects()[0])>::type>> h(200, 100);
        hist_.Refresh(&universe.getObjects(), [=](const Planet& p) { return p.getMass(); });
        //hist_.Refresh(universe.getObjects())
    }
    ++c_;
    Refresh();

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
    prop_.RemoveReaderWriter(PlanetPropertyReaderWriter::instance());
}

void CLionTestFrame::Start() {
    prop_.UpdateReaders();
    tim_.Start(this->simulation_intervall);
    tm_.Start();
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
    hist_.setRecalibrate_(true);
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
    Stop();
    prop_.ReadConfigFile(Properties::filename_, prop_.configname_);
    prop_.UpdateReaders();
    Start();
}
