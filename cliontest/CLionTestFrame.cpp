//
// Created by Arnold Müller on 11.02.17.
//

#include "CLionTestFrame.h"

#include <boost/format.hpp>

CLionTestFrame::CLionTestFrame() : wxFrame(NULL, wxID_ANY, "Hallo"), tim_(this) {
    //CreateStatusBar();

    wxMenu* menuA = new wxMenu;
    menuA->Append(ID_Menu_Restart, "&Restart", "Restart universe with new, random setup");
    menuA->Append(ID_Menu_Refill, "Re&fill", "Refill universe with random planets ");
    menuA->Append(ID_Menu_Stop, "&Pause", "Pause execution");
    menuA->Append(ID_Menu_Start, "&Continue", "Continue execution");
    menuA->AppendSeparator();
    menuA->Append(ID_Menu_Reload, "Re&load", "Reload configuration");
    menuA->AppendSeparator();
    menuA->Append(ID_Menu_ForceField, "&Toggle Force Field", "Show or hide force field");
    menuA->Append(ID_Menu_FF_Incr, "&Increase Force Field columns", "Compute more columns");
    menuA->Append(ID_Menu_FF_Decr, "&Decreade Force Field columns", "Compute less columns");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuA, "&File");
    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &CLionTestFrame::OnRestart, this, ID_Menu_Restart);
    Bind(wxEVT_MENU, &CLionTestFrame::OnStop, this, ID_Menu_Stop);
    Bind(wxEVT_MENU, &CLionTestFrame::OnStart, this, ID_Menu_Start);
    Bind(wxEVT_MENU, &CLionTestFrame::OnReload, this, ID_Menu_Reload);
    Bind(wxEVT_MENU, &CLionTestFrame::OnRefill, this, ID_Menu_Refill);
    Bind(wxEVT_MENU, &CLionTestFrame::OnForceField, this, ID_Menu_ForceField);
    Bind(wxEVT_MENU, &CLionTestFrame::OnForceFieldIncr, this, ID_Menu_FF_Incr);
    Bind(wxEVT_MENU, &CLionTestFrame::OnForceFieldDecr, this, ID_Menu_FF_Decr);

    SetSize(900, 600);
    Bind(wxEVT_PAINT, &CLionTestFrame::OnPaint, this, wxID_ANY);
    Bind(wxEVT_TIMER, &CLionTestFrame::OnTimer, this, wxID_ANY);
    prop_.AddReaderWriter(*this);
    prop_.AddReaderWriter(universe);
    prop_.AddReaderWriter(PlanetPropertyReaderWriter::instance());
    prop_.generate_pos_xmax = GetClientSize().x;
    prop_.generate_pos_ymax = GetClientSize().y;

    this->SetBackgroundColour(wxColour(0x09, 0x09, 0x20));
    Restart();
}

void CLionTestFrame::OnPaint(wxPaintEvent &event) {
    wxPaintDC dc(this);
    // {
    //     // draw axis
    //     auto size = GetClientSize();
    //     dc.SetPen(wxColour(0x30, 0x30, 0x90));
    //     dc.DrawLine(0, 0, size.GetWidth(), 0);
    //     dc.DrawLine(0, 0, 0, size.GetHeight());
    // }
    if (prop_.view_forcefield_show) {
        // draw force field
        auto size = GetClientSize();
        const float maxx = size.GetWidth();
        const float maxy = size.GetHeight();
        const float fields = prop_.view_forcefield_columns;
        const float step = size.GetWidth() / fields;
        const float step2 = step / 2.0f;
        int rx = 0, ry = 0;
        // Draw chess board background
        dc.SetPen(wxNullPen);
        for(float x = step2; x <= maxx; x += step)
        {
            ++rx;
            for(float y = step2; y <= maxy; y += step)
            {
                ++ry;
                dc.SetBrush(wxBrush(wxColour(0x09, 0x09, 0x09 + ((rx+ry)%2) * 16)));
                dc.DrawRectangle(x - step2, y - step2, x + step2, y + step2);
            }
        }
        // Draw force field vectors
        dc.SetPen(wxColour(0x30, 0x30, 0x90));
        for(float x = step2; x <= maxx; x += step)
        {
            ++rx;
            for(float y = step2; y <= maxy; y += step)
            {
                ++ry;
                Vector<float> v(x, y);
                Vector<float> force;
                for(size_t i = 0; i < universe.getActiveObjects(); ++i)
                {
                    auto const & obj = universe.getObjects()[i];
                    auto dir = obj.getPos() - v;
                    float f = obj.getMass() / std::max(1.0f, dir.length_norm()) * prop_.universe_g;
                    dir.normalize() *= f;
                    force += dir;
                }
                force *= std::pow(force.length_norm(), 0.15);
                dc.DrawLine(x, y, x + force.getX(), y + force.getY());
            }
        }
    }
    //dc.DrawText("Hallo", s.GetWidth()/2, s.GetHeight()/2);
    // {
    //     // draw histogram
    //     dc.DrawLine(5, 102, 4 + 100, 102);
    //     unsigned hx = 0;
    //     for (const auto& h : hist_) {
    //         dc.DrawLine(5 + hx, 100, 5 + hx, 100 - std::get<3>(h));
    //         ++hx;
    //     }
    // }
    for(size_t i = 0; i < universe.getActiveObjects(); ++i) {
        auto& o = universe.getObjects()[i];
        dc.SetPen(wxColour(o.getRgb_()).ChangeLightness(120));
        dc.SetBrush(wxBrush(wxColour(o.getRgb_())));
        //dc.DrawPoint(o.getPos().getX(), o.getPos().getY());
        dc.DrawCircle(o.getPos().getX(), o.getPos().getY(), o.getR());
        //dc.SetPen(wxColour(o.getRgb_()).ChangeLightness(150));
        // dc.DrawLine(o.getPos().getX(), o.getPos().getY(), o.getPos().getX() + o.getSpeed().getX(), o.getPos().getY() + o.getSpeed().getY());
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
        double total = tm_.TotalDuration().count() / 1'000'000'000.0;
        double delta_t = total - lastt;
        double delta_c = c_ - lastc;
        double framerate = delta_c / delta_t;
        lastt = total;
        lastc = c_;
        this->SetTitle(wxString::Format("%d Objekte; %3.1f Frames/Sekunde", universe.getNr_(), framerate));
        //hist_.Refresh(&universe.getObjects(), [=](const Planet& p) { return p.getMass(); });
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

void CLionTestFrame::OnRefill(wxCommandEvent &event) {
    Stop();
    prop_.generate_pos_xmax = GetClientSize().x;
    prop_.generate_pos_ymax = GetClientSize().y;
    universe.Refill();
    hist_.setRecalibrate_(true);
    Start();
}

void CLionTestFrame::OnForceField(wxCommandEvent &event) 
{
    prop_.view_forcefield_show = !prop_.view_forcefield_show;
}

void CLionTestFrame::OnForceFieldIncr(wxCommandEvent &event) 
{
    prop_.view_forcefield_columns += 2;
}

void CLionTestFrame::OnForceFieldDecr(wxCommandEvent &event) 
{
    if (prop_.view_forcefield_columns > 2)
        prop_.view_forcefield_columns -= 2;
}